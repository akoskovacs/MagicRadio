#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */

#include <avr/pgmspace.h>   /* required by usbdrv.h */
#include "usbdrv.h"
#include "oddebug.h"        /* This is also an example for using debug macros */
#include "requests.h"       /* The custom request numbers we use */

static uint16_t current_frequency = 440;

void radio_transmit(uint16_t freq)
{
    TCCR1B |= _BV(CS10); /* Give it the div1 prescaler */
    OCR1A   = F_CPU/freq-1;
}

void radio_disable(void)
{
    /* Just cut off the prescaler */
    TCCR1B &= (uint8_t)~_BV(CS10);
    PORTB  &= (uint8_t)~_BV(PB2);
}

void led_pwm_enable(uint8_t pwmval)
{
    TCCR0A |= _BV(COM0A0); /* Connect to PB2 */
    OCR0A   = pwmval;
}

void led_pwm_disable(void)
{
    TCCR0A &= (uint8_t)~_BV(COM0A0); /* Disconnect to PB2 */
}

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
    usbRequest_t    *rq = (void *)data;
    
    static uchar    usb_ret_val = 0;

    usbMsgPtr = &usb_ret_val;
    switch (rq->bRequest) {
        case CMD_GET_FREQUENCY:
        usbMsgPtr = (uint8_t *)&current_frequency;
        return sizeof(current_frequency);

        case CMD_TX_ON:
        current_frequency = rq->wValue.word;
        radio_transmit(current_frequency);
        return sizeof(usb_ret_val);

        case CMD_TX_OFF:
        radio_disable();
        return sizeof(usb_ret_val);

        case CMD_LED_ON:
        led_pwm_enable(rq->wValue.word);
        return sizeof(usb_ret_val);

        case CMD_LED_OFF:
        led_pwm_disable();
        return sizeof(usb_ret_val);

        /* No such command -> error */
        default:
        usb_ret_val = 1;
        return sizeof(usb_ret_val);
    }
}

/* ------------------------------------------------------------------------- */

void init(void)
{
    /* LED and "radio" pins are output */
    DDRB  |= _BV(PB2) | _BV(PB3);

    /* Init the 16bit timer for radio transmit */
    /* OC1A as output PB3 */
    TCCR1A = _BV(COM1A0);     /* Toggle on CTC */
    TCCR1B = _BV(WGM12);      /* CTC mode      */
    /* Do not start the timer, yet. Only if the host commands it. */

    /* Init the 8bit timer for the LED dimming  */
    /* Toggle on compare match (fast PWM) */
    TCCR0A = _BV(WGM01)|_BV(WGM00);
    TCCR0B = _BV(WGM02)|_BV(CS01)|_BV(CS00); /* div64 */
    led_pwm_enable(150); /* LED PWM enabled by default */
}

int __attribute__((noreturn)) main(void)
{
    uint8_t i = 0;
    init();
    wdt_enable(WDTO_1S);
    odDebugInit();
    DBG1(0x00, 0, 0);       /* debug output: main starts */
    usbInit();
    usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
    while(--i){             /* fake USB disconnect for > 250 ms */
        wdt_reset();
        _delay_ms(1);
    }
    usbDeviceConnect();
    init();
    sei();
    DBG1(0x01, 0, 0);       /* debug output: main loop starts */
    for(;;){                /* main event loop */
        DBG1(0x02, 0, 0);   /* debug output: main loop iterates */
        wdt_reset();
        usbPoll();
    }
}
