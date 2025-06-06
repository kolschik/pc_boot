QNX_TARGET=/opt/qnx650/target/qnx6
QNX_HOST=/opt/qnx650/host/linux/x86
QNX_CONFIGURATION=/etc/qnx
MAKEFLAGS=-I/opt/qnx650/target/qnx6/usr/include
PATH=/usr/local/sbin:/usr/local/bin:/usr/bin:/opt/qnx650/host/linux/x86/usr/bin:/opt/qnx650/.qnx/bin:/opt/qnx650/jre/bin

export QNX_TARGET QNX_HOST QNX_CONFIGURATION MAKEFLAGS PATH
