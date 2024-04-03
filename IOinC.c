struct PIT_t {
      volatile unsigned int      DR;
      volatile unsigned int      DIR;
      volatile unsigned int      MASK;
      volatile unsigned int      EDGE;
      };


// The LED pit is at this base address
struct PIT_t      * const ledp = ((struct PIT_t *) 0xFF200000);
// The BUTTONS pit is at this base address
struct PIT_t      *const buttonp = ((struct PIT_t *) 0xFF200050);
// The HEX digits 0 through 3 PIT
struct PIT_t      *const hex03p = ((struct PIT_t *)0xFF200020);
// The Swicthes PIT
struct PIT_t      *const swp = ((struct PIT_t *)0xFF200040);

void
HEXaroundD() {
    volatile int *HEX_3_0 = 0xFF200020;
    int pattern, counter, delay;
    
    while (1) {
        pattern = 1;
        counter = 6;
        while (counter > 0) {
            *HEX_3_0 = pattern;     // display pattern
            pattern = pattern << 1; // shift pattern
        
            delay = 800000;
            while (delay > 0)       // wait awhile
                delay = delay - 1;
        
            counter = counter - 1;  // count down
        }        
    }
}

void
HEXaround() {
    int pattern, counter, delay;
    
    while (1) {
        pattern = 1;
        counter = 6;
        while (counter > 0) {
            hex03p->DR = pattern;     // display pattern
            pattern = pattern << 1; // shift pattern
        
            delay = 800000;
            while (delay > 0)       // wait awhile
                delay = delay - 1;
        
            counter = counter - 1;  // count down
        }        
    }
}

// buttons turn on leds/to exit press all buttons
void
LEDButtonsAll(void) {
	unsigned int b;
   do {
	   b = buttonp->DR;
	   ledp->DR = b;
   } while (b != 0xF);
 }	

void
LEDButton0Toggle(void) {
   ledp->DR = 0; // turn off all leds
   while (1) {
       // check if the first button was pressed and released
      if (buttonp->EDGE & 0x1) {
         ledp->DR = ledp->DR ^ 0x1; // toggle bit 0 of the leds
         buttonp->EDGE = 0x1; // reset bit 0 of the BUTTON EDGE
      }
   }
}	

char seg7[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x67, 0x77, 
					0x7c, 0x39, 0x5e, 0x79, 0x71};

void
HEXSwitches() {
	unsigned int value;
	
	while(1) {
		value = swp->DR;		 	// read the SW slider switch values
		ledp->DR = value; 		// light up the red LEDs

		hex03p->DR = seg7[value & 0xF] | seg7[value >> 4 & 0xF] << 8 | 
			seg7[value >> 8] << 16;
	}
}

void
HEXpattern() {
	
    int pattern, counter, delay;
    
    while (1) {
        pattern = 1;
        counter = 6;
        while (counter > 0) {
            hex03p->DR = pattern;     // display pattern
            pattern = pattern << 1; // shift pattern
        
            delay = 800000;
            while (delay > 0)       // wait awhile
                delay = delay - 1;
        
            counter = counter - 1;  // count down
        }        
    }
}

main()
{
     HEXaroundD();
	HEXaround();
	HEXpattern();
	HEXSwitches();
	LEDButtonsAll();
	LEDButton0Toggle();
}