
BIN=karuta-server
PIDFILE=/var/run/$BIN.pid
LOGFILE=$LOGDIR/access.log

check_status() {
	if [ ! -f "$PIDFILE" ]; then
		return 1
	fi

	local PID=$(cat "$PIDFILE")
	if [ -z "$PID" ] || ! kill -0 "$PID" 2>/dev/null; then
		rm -f "$PIDFILE"
		return 1
	fi

	return 0
}

start() {
	if check_status; then
		echo 'karuta-server is already running.' >&2
		return 1
	fi
	echo 'Starting karuta-server...' >&2
	cd $ROOTDIR
	local CMD="./$BIN --service > /dev/null & echo \$!"
	su -c "$CMD" $USER > "$PIDFILE" &
	echo 'karuta-server is running.' >&2
}

stop() {
	if ! check_status; then
		echo 'karuta-server is not running.' >&2
		return 1
	fi
	echo 'Stopping karuta-server...' >&2
	kill -15 $(cat $PIDFILE)
	rm -f "$PIDFILE"
	echo 'karuta-server is stopped.' >&2
}

status() {
	if check_status; then
		echo 'karuta-server is running.' >&2
	else
		echo 'karuta-server is not running.' >&2
	fi
}

case "$1" in
start)
	start
	;;
stop)
	stop
	;;
status)
	status
	;;
retart)
	stop
	start
	;;
*)
	echo "Usage: $0 {start|stop|restart|status}"
esac
