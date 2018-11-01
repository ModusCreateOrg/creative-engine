# -*- mode: ruby -*-
# vi: set ft=ruby :

# This Vagrantfile is here to make it easier to test the build scripts with
# a variety of operating systems, including Ubuntu 18.04 and Arch linux.

box = ENV["BOX"] || "bento/ubuntu-18.04"
# To build with arch linux, run:
#      BOX=archlinux/archlinux vagrant up
Vagrant.configure("2") do |config|
  config.vm.box = box
  config.vm.network "private_network", ip: "192.168.50.4"
  # These are type "nfs" because when using vboxsf on archlinux/archlinux
  # cmake blocks on:
  # openat(AT_FDCWD, "/vagrant/build/CMakeFiles/3.12.2/CompilerIdC/CMakeCCompilerId.c", O_WRONLY|O_CREAT|O_TRUNC, 0666) = 3
  # writev(3, [{iov_base=NULL, iov_len=0}, {iov_base="#ifdef __cplusplus\n# error \"A C+"..., iov_len=18988}], 2
  # See https://www.virtualbox.org/ticket/17757
  config.vm.synced_folder "../creative-engine", "/creative-engine", type: "nfs"
  config.vm.synced_folder ".", "/vagrant", type: "nfs"
  config.vm.provision "shell", inline: "/vagrant/scripts/build.sh clean"
end
