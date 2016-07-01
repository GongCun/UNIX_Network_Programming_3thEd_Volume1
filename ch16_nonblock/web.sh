#!/bin/bash

nconn=$1
[ -z "$nconn" ] && {
  echo "Usage: $0 <nconn>" >&2
  exit 1
}

Dir=`dirname $0`
cd $Dir

./web $nconn www.firstview.com / \
    /themes/default/images/blogtitle-logo.jpg \
    /themes/default/images/blog-thumb3.jpg \
    /themes/default/images/blogfeaturestrends.jpg \
    /themes/default/images/logo75pc.gif \
    /themes/default/images/blogtitle-logo.jpg \
    /themes/default/images/logo75pc_w_on_b.gif
