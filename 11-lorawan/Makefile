# ------------ [Task 1] --------------------
# Replace these values with your own from the registered device
# No spaces between the quotes
CFLAGS += -DCONFIG_LORAMAC_APP_EUI_DEFAULT=\"0000000000000000\"
CFLAGS += -DCONFIG_LORAMAC_DEV_EUI_DEFAULT=\"XXXXXXXXXXXXXXXX\"
CFLAGS += -DCONFIG_LORAMAC_APP_KEY_DEFAULT=\"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\"
# -----------------------------------------

APPLICATION = lorawan_example

# If no BOARD is found in the environment, use this default:
BOARD ?= b-l072z-lrwan1

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../RIOT

# include SAUL to interact with onboard sensors and actuators
USEMODULE += saul

# include and auto-initialize all available sensors
USEMODULE += saul_default

# Include board's default network devices and auto-initialization of GNRC
# interfaces
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif

# Add support for GNRC LoRaWAN
USEMODULE += gnrc_lorawan

# Use GNRC pktdump to print downlink messages
USEMODULE += gnrc_pktdump

# Include the shell and shell commands.
USEMODULE += shell
USEMODULE += shell_cmds_default

# Use GNRC Txtsnd to transmit LoRaWAN from the shell
USEMODULE += gnrc_txtsnd

# Some workarounds are needed in order to get the tutorial running on
# some computers.
-include ../lab_workarounds.mk

# we use ztimer to read sensors periodically
USEMODULE += ztimer
USEMODULE += ztimer_sec

# We want to use OTAA join procedure
CFLAGS += -DCONFIG_LORAMAC_DEFAULT_JOIN_PROCEDURE_OTAA
# CFLAGS += -DCONFIG_LORA_SYMBOL_TIMEOUT_DEFAULT=20

# Bring the LoRa radio driver
USEMODULE += sx127x

include $(RIOTBASE)/Makefile.include
