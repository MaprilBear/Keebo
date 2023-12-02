// frequency reload values
// want to interrupt 256 times per period
// value = 80MHz / (256 * frequency (Hz))
#define C 598


// duration definitions (120 BPM)
// taken from Lab3, have not checked for fucntan
#define WHOLE          160000000
#define HALF            80000000
#define QUARTER         40000000
#define EIGTH           20000000
#define SIXTEENTH       10000000
#define THIRTYSECOND     5000000
#define SIXTYFOURTH      2500000
#define ONETWENTYEIGHTH  1250000

void Song_Init(void);
void Song_Start(void);