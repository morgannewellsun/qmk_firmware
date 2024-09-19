#pragma once

#define DEFAULT_TAP_TERM 175
#define DEFAULT_MULTITAP_TERM 175

#define SUPERKEY_RANGE_START SAFE_RANGE
#define SK_INDEX(KC) (KC - SUPERKEY_RANGE_START)
#define SK_DEFINE(_keycode, _alias_keycode, _tap_term, _multitap_term, _down, _up, _interrupt, _timeout) \
    [SK_INDEX(_keycode)] = { .config = {.keycode = _keycode, .alias_keycode = _alias_keycode, .tap_term = _tap_term, .multitap_term = _multitap_term, .down = _down, .up = _up, .interrupt = _interrupt, .timeout = _timeout} }

struct keyboard_state_t;

typedef struct superkey_state_t {   
    uint32_t multitap_start_time;
    uint32_t tap_start_time;
    uint16_t interrupting_keycode;
    uint8_t interrupt_result;
    uint8_t timeout_result;
    uint8_t multitap_result;
    bool interrupt_is_on;
    bool timeout_is_on;
    bool prev_was_tap;
} superkey_state_t;

typedef struct superkey_config_t {  
    void (*down)(struct keyboard_state_t*, superkey_state_t*);
    bool (*interrupt)(struct keyboard_state_t*, superkey_state_t*);  // returns whether or not to interrupt
    void (*timeout)(struct keyboard_state_t*, superkey_state_t*);
    void (*up)(struct keyboard_state_t*, superkey_state_t*);
    uint16_t keycode;
    uint16_t alias_keycode;
    uint16_t multitap_term;
    uint16_t tap_term;
} superkey_config_t;

typedef struct superkey_t {
    superkey_state_t state;
    superkey_config_t config;
} superkey_t;

enum interrupt_results {
    NO_INTERRUPT = 0,
    INTERRUPT_BEFORE_TIMEOUT,
    INTERRUPT_AFTER_TIMEOUT,
};

enum timeout_results {
    NO_TIMEOUT = 0,
    TIMEOUT_BEFORE_INTERRUPT,
    TIMEOUT_AFTER_INTERRUPT,
};

enum multitap_results {
    NO_MULTITAP = 0,
    DOUBLE_TAP,
    TRIPLE_TAP,
    // you could add more if you somehow needed to (?!)
};

extern superkey_t superkeys[];
extern const size_t n_superkeys;


void superkey_init_task(void);
bool superkey_process_record_task(struct keyboard_state_t* keyboard_state, uint16_t keycode, bool pressed);
void superkey_matrix_scan_task(struct keyboard_state_t* keyboard_state);
void superkey_inject_interrupt(struct keyboard_state_t* keyboard_state, uint16_t superkey_keycode, uint16_t interrupting_keycode);
