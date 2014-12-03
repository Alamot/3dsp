#Patched 3DSP USB driver (tested on Ubuntu 13.04 and 14.04 with network manager, wifi is working, bluetooth is not)

a) First, you should install all dependencies:

    $ sudo apt-get install build-essential linux-headers-`uname -r`


**********IF YOU HAVE UBUNTU 14.04+ JUMP TO STEP (f).*****************



b) Download and install the library libnotify1 through the link:

http://packages.ubuntu.com/lucid/libnotify1

You can download the library libnotify1 with the wget tool: 

x86_64: 

    $ wget -c http://mirror.pnl.gov/ubuntu//pool/main/libn/libnotify/libnotify1_0.4.5-1ubuntu3_amd64.deb

x86_32:

    $ wget -c http://mirror.pnl.gov/ubuntu//pool/main/libn/libnotify/libnotify1_0.4.5-1ubuntu3_i386.deb 

c) Install the library

x86_64:  

    $ sudo dpkg-i libnotify1_0.4.5-1ubuntu3_amd64.deb 

x86_32: 

    $ sudo dpkg-i libnotify1_0.4.5-1ubuntu3_i386.deb 

d) Copy the library libnotify1 to the respective folder (x86_64 / x86_32)

x86_64: 

    $ sudo cp /usr/lib/libnotify.so.1.1.3 /usr/lib/x86_64-linux-gnu/  

x86_32: 

    $ sudo cp /usr/lib/libnotify.so.1.1.3 /usr/lib/i386-linux-gnu/
  
e) make symlink with the command:

x86_64: 

    $ sudo ln-sf /usr/lib/x86_64-linux-gnu/libnotify.so.1.1.3 /usr/lib/x86_64-linux-gnu/libnotify.so.1 

x86_32:

    $ sudo ln-sf /usr/lib/i386-linux-gnu/libnotify.so.1.1.3 /usr/lib/i386-linux-gnu/libnotify.so.1 


*******************************************************************************************************


f) With all dependencies satisfied, just clone this git repository and run Install\_3DSP.sh:

    $ git clone http://github.com/Alamot/3dsp.git
    $ cd 3dsp
    $ sudo bash Install_3DSPUSB.sh

If you don't use git, just download the source from Github, extract it and run Install\_3DSP.sh:

    $ curl http://github.com/Alamot/3dsp/tarball/master -o 3dsp.tar.gz
    $ tar -xvzf 3dsp.tar.gz
    $ cd 3dsp
    $ sudo bash Install_3DSP.sh 

g) reboot your system & enjoy. :)


[ACKNOWLEDGEMENTS]

This patch is due to many people:

Reyiyo: https://github.com/reyiyo/3dsp
    
Eriton Benevides: http://eritonb.blogspot.gr/2012/05/3dsp-bluew2310-u-no-ubuntu-1204-kernel.html
    
Aku-Aku: http://ubuntuforums.org/showthread.php?t=1234213&page=5 
    
ThankS! :)
