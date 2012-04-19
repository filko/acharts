#! /bin/bash

echo aclocal
aclocal || exit 127

echo autoconf
autoconf || exit 127

echo automake --add-missing --copy
automake --add-missing --copy || exit 127
