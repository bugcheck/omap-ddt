#!/bin/sh

TESTCASEID=$1
MODE=$2

case "$TESTCASEID" in
	# Transfer to Existing Device
	1)
		verifyspeed.sh $MODE || exit 1 && for i in $I2C_ADDRESSES; do i2cdump -y $I2C_ADAPTER_1 $i b || exit 1; done
		verifyspeed.sh $MODE || exit 1 && testrw.sh $I2C_ADAPTER_1 $I2C_REG_1 $I2C_REG_1_VALUE_INITIAL || exit 1
		verifyspeed.sh $MODE || exit 1 && for i in $I2C_ADDRESSES; do i2cdump -y $I2C_ADAPTER_1 $i b || exit 1; done
	;;
	# Transfer to Multiple Devices
	2)
		verifyspeed.sh $MODE || exit 1 && for i in $I2C_ADDRESSES; do i2cdump -y $I2C_ADAPTER_1 $i b || exit 1; done
		verifyspeed.sh $MODE || exit 1 && (testrw.sh $I2C_ADAPTER_1 $I2C_REG_1 $I2C_REG_1_VALUE_INITIAL &) && testrw.sh $I2C_ADAPTER_1 $I2C_REG_2 $I2C_REG_2_VALUE_INITIAL && wait.sh testrw.sh 1 || exit 1
		verifyspeed.sh $MODE || exit 1 && for i in $I2C_ADDRESSES; do i2cdump -y $I2C_ADAPTER_1 $i b || exit 1; done
	;;
	# Transfer to Non Existing Device
	3)
		verifyspeed.sh $MODE || exit 1 && for i in $I2C_ADDRESSES; do i2cdump -y $I2C_ADAPTER_1 $i b || exit 1; done
		verifyspeed.sh $MODE || exit 1 && (testrw.sh $I2C_ADAPTER_1 $I2C_REG_INVALID $I2C_REG_1_VALUE_INITIAL &) && testrw.sh $I2C_ADAPTER_1 $I2C_REG_INVALID $I2C_REG_2_VALUE_INITIAL && wait.sh testrw.sh 1 && exit 1
		verifyspeed.sh $MODE || exit 1 && for i in $I2C_ADDRESSES; do i2cdump -y $I2C_ADAPTER_1 $i b || exit 1; done
	;;
	# Transfer Cancellation to Existing Device
	4)
		verifyspeed.sh $MODE || exit 1 && for i in $I2C_ADDRESSES; do i2cdump -y $I2C_ADAPTER_1 $i b || exit 1; done
		verifyspeed.sh $MODE || exit 1 && ((testrw.sh $I2C_ADAPTER_1 $I2C_REG_1 $I2C_REG_1_VALUE_INITIAL &) && testrw.sh $I2C_ADAPTER_1 $I2C_REG_2 $I2C_REG_2_VALUE_INITIAL &) && killall testrw.sh || exit 1
		verifyspeed.sh $MODE || exit 1 && for i in $I2C_ADDRESSES; do (i2cdump -y $I2C_ADAPTER_1 $i b &) && killall i2cdump || exit 1; done
		verifyspeed.sh $MODE || exit 1 && for i in $I2C_ADDRESSES; do i2cdump -y $I2C_ADAPTER_1 $i b || exit 1; done
	;;
	# Transfer using Interrupt Mode
	5)
		verifyspeed.sh $MODE || exit 1 && for i in $I2C_ADDRESSES; do i2cdump -y $I2C_ADAPTER_1 $i b || exit 1; done
		verifyspeed.sh $MODE || exit 1 && echo `cat $PROCFS_INTERRUPTS | grep $INT_24XX_I2C1_IRQ: | sed -e "s/ */ /g" | cut -d ' ' -f3` > $TEMP_FILE_1 || exit 1
		verifyspeed.sh $MODE || exit 1 && for i in $I2C_ADDRESSES; do i2cdump -y $I2C_ADAPTER_1 $i b || exit 1; done
		verifyspeed.sh $MODE || exit 1 && echo `cat $PROCFS_INTERRUPTS | grep $INT_24XX_I2C1_IRQ: | sed -e "s/ */ /g" | cut -d ' ' -f3` > $TEMP_FILE_2 || exit 1
		verifyspeed.sh $MODE || exit 1 && cat $TEMP_FILE_1 && cat $TEMP_FILE_2; cmp $TEMP_FILE_1 $TEMP_FILE_2 && exit 1
		verifyspeed.sh $MODE || exit 1 && for i in $I2C_ADDRESSES; do i2cdump -y $I2C_ADAPTER_1 $i b || exit 1; done
	;;
	*)
		echo hola;
	;;
esac

# End of file
