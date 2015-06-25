RASPPI ?= 2
ifeq ($(OS),Windows_NT)
   PATH=$(CIRCLEHOME)/tools/arm-bcm2708/gcc-linaro-arm-none-eabi-4.8-2014.04/bin
   PREFIX ?= arm-none-eabi-
   REMOVE ?= del
else
   export PATH=$PATH:${PWD}/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin
   PREFIX ?= arm-linux-gnueabihf-
   REMOVE ?= rm -f
endif
