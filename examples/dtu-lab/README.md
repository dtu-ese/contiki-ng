# Contiki-NG Lab: Installation Guide

This guide installs the Contiki-NG tool chain on a Linux Virtual Machine using Vagrant and VirtualBox. The installation is configurated for using Contiki-NG with the **CC2650 Launchpad**.

It is strongly recommended to finish the installation **before** the course. The process takes several minutes.

### Step 1: Install VirtualBox
Download and install the latest VirtualBox for you host system: https://www.virtualbox.org/wiki/Downloads.

### Step 2: Install VirtualBox Extension Pack
Download and install the latest VirtualBox Extension Pack: https://www.virtualbox.org/wiki/Downloads. This step is required for accessing USB devices from within the VM.

You shall add the Extension Pack from Preferences -> Extensions -> Add new package.

**Important**: The versions of the VirtualBox and VirtualBox Extension Pack should match.

### Step 3: Install Vagrant
Download and install Vagrant: https://www.vagrantup.com/downloads.html.

### Step 4: Clone Contiki-NG

Clone the repository:

```sh
$ git clone --depth 50 git@github.com:dtu-ese/contiki-ng.git
```

Initialise the submodules:

```sh
$ cd contiki-ng
$ git submodule update --init --recursive
```

### Step 5: Start and log into the VM
Start the VM:

```sh
$ cd tools/vagrant
$ vagrant up
```
**Important**: The first time that you will do this step it will take several minutes.

SSH in the VM:

```sh
$ vagrant ssh
```

### Step 6: Hello World
Navigate to the hello-world example:

```sh
$ cd contiki-ng/examples/hello-world/
```

Build the example for Linux (native):

```sh
$ make TARGET=native
```

Run the example:

```sh
$ ./hello-world.native
```

The output should like like this:

```sh
[INFO: Main      ] Starting Contiki-NG-develop/v4.4-31-ge14d7dc
[INFO: Main      ] - Routing: RPL Lite
[INFO: Main      ] - Net: tun6
[INFO: Main      ] - MAC: nullmac
[INFO: Main      ] - 802.15.4 PANID: 0xabcd
[INFO: Main      ] - 802.15.4 Default channel: 26
[INFO: Main      ] Node ID: 1800
[INFO: Main      ] Link-layer address: 0102.0304.0506.0708
[INFO: Main      ] Tentative link-local IPv6 address: fe80::302:304:506:708
[INFO: Native    ] Added global IPv6 address fd00::302:304:506:708
Hello, world
```

***Note:*** This step builds the hello-world example for Linux. During the course, we will build it and run it on the Launchpad CC2650 platform (ARM Cortex-M3 processor).

