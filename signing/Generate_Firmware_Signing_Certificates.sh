#!/bin/sh
# Simply put key in the forlder and the IDE will sign binaries
openssl genrsa -out private.key 2048
openssl rsa -in private.key -outform PEM -pubout -out public.key
