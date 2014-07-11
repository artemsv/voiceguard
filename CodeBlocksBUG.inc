
#ifdef NEWVG

	#define DAT			_BV(PF2)		//  АЦП-вход встроенного УМВД
	#define PWGood		_BV(PF3)		//
	#define RELE		_BV(PD5)
	#define TELON		_BV(PE3)
	#define TELUP		PORTE |= TELON
	#define TELDOWN		PORTE &= ~TELON
	#define DDR_TELON	DDRE
#else
	#define TELON		_BV(PF3)
	#define TELUP		PORTF |= TELON
	#define TELDOWN		PORTF &= ~TELON
	#define DDR_TELON	DDRF
#endif

/*
#ifndef NEWVG
	#define		VK_1		1
	#define		VK_2		2
	#define		VK_3		3
	#define		VK_GUARD	4
	#define		VK_4		5
	#define		VK_5		6
	#define		VK_6		7
	#define		VK_TEL		12
	#define		VK_7		9
	#define		VK_8		10
	#define		VK_9		11
	#define		VK_SETUP	8
	#define		VK_CANCEL	13
	#define		VK_0		14
	#define		VK_SHARP	15
	#define		VK_FUNC		16
	#define		VK_HF		17
	#define		VK_LEFT		12
	#define		VK_RIGHT	16
	#define		VK_REDIAL	20

	#define		VK_UP			VK_LEFT
	#define		VK_DOWN			VK_RIGHT
	#define 		VK_BACKSPACE	VK_FUNC
#else
*/
	//  клавиатура уже всегда будет работать по новому
	#define		VK_1		17
	#define		VK_2		18
	#define		VK_3		19
	#define		VK_GUARD	20
	#define		VK_4		5
	#define		VK_5		6
	#define		VK_6		7
	#define		VK_7		9
	#define		VK_8		10
	#define		VK_9		11
	#define		VK_SETUP	8
	#define		VK_CANCEL	13
	#define		VK_0		14
	#define		VK_SHARP	15
	#define		VK_LEFT		12
	#define		VK_RIGHT	16

	#define		VK_UP			VK_LEFT
	#define		VK_DOWN			VK_RIGHT
	#define 		VK_BACKSPACE	VK_SETUP
//#endif
