#!/bin/bash
cd build/classes
export CLASSPATH=$CLASSPATH:/usr/share/java/mysql.jar
java raimServer.Listener

