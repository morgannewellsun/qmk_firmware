
# compilation stuff
SRC += features.c
SRC += intercepts.c
SRC += mouse_passthrough_receiver.c
SRC += rgb_engine.c
SRC += superkeys.c
SRC += timeouts.c
LTO_ENABLE = yes
OPT = 3  # -O3 optimization

# qmk features necessary for keymap
KEY_OVERRIDE_ENABLE = yes
POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = custom
RAW_ENABLE = yes
RGBLIGHT_ENABLE = yes

# other qmk features
AUDIO_ENABLE = no
BLUETOOTH_ENABLE = no
BOOTMAGIC_ENABLE = no
COMBO_ENABLE = no
COMMAND_ENABLE = no
CONSOLE_ENABLE = no
DYNAMIC_TAPPING_TERM_ENABLE = no
ENCODER_ENABLE = no
EXTRAKEY_ENABLE = no
LEADER_ENABLE = no
MAGIC_ENABLE = no
MIDI_ENABLE = no
MOUSEKEY_ENABLE = no
NKRO_ENABLE = yes
OLED_ENABLE = no
RING_BUFFERED_6KRO_REPORT_ENABLE = no
UNICODE_ENABLE = no
