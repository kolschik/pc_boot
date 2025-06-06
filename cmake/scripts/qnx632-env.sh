QNX_TARGET=/opt/qnx632/target/qnx6
QNX_HOST=/opt/qnx632/host/linux/x86
QNX_CONFIGURATION=/etc/qnx
MAKEFLAGS=-I/opt/qnx632/target/qnx6/usr/include
PATH=/usr/local/sbin:/usr/local/bin:/usr/bin:/opt/qnx632/host/linux/x86/usr/bin:/opt/qnx632/.qnx/bin:/opt/qnx632/jre/bin

export QNX_TARGET QNX_HOST QNX_CONFIGURATION MAKEFLAGS PATH
