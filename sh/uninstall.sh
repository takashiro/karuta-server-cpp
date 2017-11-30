#!/bin/bash

service karuta-server stop

rm -f /etc/init.d/karuta-server
update-rc.d karuta-server remove

rm -rf /opt/karuta-server
