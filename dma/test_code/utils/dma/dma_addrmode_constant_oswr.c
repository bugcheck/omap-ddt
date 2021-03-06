/*
 *  DMA OSWR feature testing module
 *
 *  Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
 *          - G, Manjunath Kondaiah <manjugk@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#include <linux/delay.h>

#include "dma_single_channel.h"

#define TRANSFER_COUNT 1
#define TRANSFER_POLL_COUNT 60
#define TRANSFER_POLL_TIME 1500

u32 reg_dump_after_config[EDMA_CH_END * 4];

u32 reg_dump_after_suspend[EDMA_CH_END*4];
EXPORT_SYMBOL(reg_dump_after_suspend);

u8 wait_oswr_trigger;
EXPORT_SYMBOL(wait_oswr_trigger);

static struct dma_transfer transfers[TRANSFER_COUNT];
/*
 * Checks that the destination buffers were written correctly
 */
static void check_test_passed(void){
     int i;
     int error = 0;
     /* Check that all the transfers finished */
     for(i = 0; i < TRANSFER_COUNT; i++){
         if(!transfers[i].data_correct){
             error = 1;
             printk("Transfer id %d failed\n", transfers[i].transfer_id);
         }
     }

     if(!error) {
         set_test_passed(1);
     } else {
         set_test_passed(0);
     }
}

/*
 * Function used to verify the source an destination buffers of a dma transfer
 * are equal in content
 */
int verify_buffers(struct dma_buffers_info *buffers) {
    int i;
    u8 *src_address = (u8*) buffers->src_buf;
    u8 *dest_address = (u8*) buffers->dest_buf;

	for (i = 0; i < buffers->buf_size; i++) {
		if (src_address[i] != dest_address[i]) {
			printk(KERN_ERR "\nError: Data mismatch\n");
			printk(KERN_ERR "src_buf[%d]=%d dest_buf[%d]=%d\n",
			i, src_address[i], i, dest_address[i]);
			return 1;
		}
	}
	return 0;
}

/*
 * Callback function that dma framework will invoke after transfer is done
 */
void dma_callback(int transfer_id, u16 transfer_status, void *data)
{
	struct dma_transfer *transfer = (struct dma_transfer *) data;
	int error = 1;
	transfer->data_correct = 0;
	transfer->finished = 1;
	printk(KERN_INFO "Transfer complete for id %d, checking destination buffer\n",
	   transfer->transfer_id);
	/* Check if the transfer numbers are equal */
	if (transfer->transfer_id != transfer_id) {
		printk(KERN_WARNING "Transfer id %d differs from the one"
		" received in callback (%d)\n", transfer->transfer_id,
		transfer_id);
	}
	unmap_phys_buffers(&transfer->buffers);
       /* Check the transfer status is acceptable */
	if ((transfer_status & OMAP_DMA_BLOCK_IRQ) || (transfer_status == 0)) {
           /* Verify the contents of the buffer are equal */
           error = verify_buffers(&(transfer->buffers));
       } else {
		printk(KERN_ERR "Error:transfer id %d has status %x\n",
			transfer->transfer_id, transfer_status);
           return;
       }

       if (error) {
		printk(KERN_ERR "Src/dest buffer mismatch for transfer id %d\n",
			transfer->transfer_id);
       } else {
		printk(KERN_INFO "Verification succeeded for transfer id %d\n",
			transfer->transfer_id);
		transfer->data_correct = 1;
       }
	wait_oswr_trigger = transfer_id;
	printk("%s: wait_oswr_trigger : %d\n", __func__, wait_oswr_trigger);
}

/*
 * Determines if the transfers have finished
 */
static int get_transfers_finished(void){
       int i = 0;
       for(i = 0; i < TRANSFER_COUNT; i++){
            if(!transfers[i].finished){
                return 0;
            }
       }
       return 1;
}

/*
 * Requests a dma transfer
 */
int request_dma(struct dma_transfer *transfer){
       int success;
       transfer->finished = 0;
       printk("\nRequesting OMAP DMA transfer\n");
       success = omap_request_dma(
               transfer->device_id,
               "dma_test",
               dma_callback,
               (void *) transfer,
               &(transfer->transfer_id));
       if (success) {
          printk(" Request failed\n");
          transfer->request_success = 0;
          return 1;
       }else{
          printk(" Request succeeded, transfer id is %d\n",
               transfer->transfer_id);
          transfer->request_success = 1;
       }
       return 0;
}


/*
 * Function called when the module is initialized
 */
static int __init dma_module_init(void) {
       int error;
       int i = 0;

	wait_oswr_trigger = -1;
	for (i = 0; i < TRANSFER_COUNT; i++) {
		/* Create the transfer for the test */
		transfers[i].device_id = OMAP_DMA_NO_DEVICE;
		transfers[i].sync_mode = OMAP_DMA_SYNC_ELEMENT;
		transfers[i].data_burst = OMAP_DMA_DATA_BURST_DIS;
		transfers[i].data_type = OMAP_DMA_DATA_TYPE_S8;
		transfers[i].endian_type = DMA_TEST_LITTLE_ENDIAN;
		transfers[i].addressing_mode = OMAP_DMA_AMODE_POST_INC;
		transfers[i].dst_addressing_mode = OMAP_DMA_AMODE_POST_INC;
		transfers[i].priority = DMA_CH_PRIO_HIGH;
		transfers[i].buffers.buf_size = (128 * (i+1)*(i+1)) + i % 2;
		transfers[i].src_ei = transfers[i].dest_ei = 0;
		transfers[i].src_fi = transfers[i].dest_fi = 0;

           /* Request a dma transfer */
           error = request_dma(&transfers[i]);
           if( error ){
               set_test_passed(0);
               return 1;
           }

           /* Request 2 buffer for the transfer and fill them */
           error = create_transfer_buffers(&(transfers[i].buffers));
           if( error ){
               set_test_passed(0);
               return 1;
           }
           fill_source_buffer(&(transfers[i].buffers));

           /* Setup the dma transfer parameters */
           setup_dma_transfer(&transfers[i]);
       }

       for(i = 0; i < TRANSFER_COUNT; i++){
           /* Start the transfers */
           start_dma_transfer(&transfers[i]);

	printk("Register Dump After configuration:\n");
	printk("DMA channel number : %d\n", transfers[i].transfer_id);
	dma_channel_registers_dump(transfers[i].transfer_id, reg_dump_after_config);
       }


       /* Poll if the all the transfers have finished */
       for(i = 0; i < TRANSFER_POLL_COUNT; i++){
            if(get_transfers_finished()){
               mdelay(TRANSFER_POLL_TIME);
               check_test_passed();
               break;
            }else{
               mdelay(TRANSFER_POLL_TIME);
            }
       }

       /* This will happen if the poll retries have been reached*/
       if(i == TRANSFER_POLL_COUNT){
           set_test_passed(0);
           return 1;
       }

       return 0;
}

/*
 * Function called when the module is removed
 */
static void __exit dma_module_exit(void) {
	int i;

	for (i = EDMA_CCR; i < EDMA_CH_END; i++) {
		if (reg_dump_after_config[i] != reg_dump_after_suspend[i]){
			printk("%s: Test failed ....!!!!!\n", __func__);
			set_test_passed(0);
			break;
		}
	}
	set_test_passed(1);
	for(i = 0; i < TRANSFER_COUNT; i++)
		stop_dma_transfer(&transfers[i]);

}

module_init(dma_module_init);
module_exit(dma_module_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
