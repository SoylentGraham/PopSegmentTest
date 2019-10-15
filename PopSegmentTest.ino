//We always have to include the library
#include "LedControl.h"

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 ***** Please set the number of devices you have *****
 But the maximum default of 8 MAX72XX wil also work.
 */
LedControl lc=LedControl(2,4,3,2);

/* we always wait a bit between updates of the display */
unsigned long delaytime=50;

namespace SegmentIndex
{
	enum TYPE
	{
		Dot	= 0,
		Top,
		TopRight,
		BottomRight,
		Bottom,
		BottomLeft,
		TopLeft,
		Middle,
		
		Count
	};
}

namespace SegmentMask
{
	const int	Dot			= 1<<SegmentIndex::Dot;
	const int	Top			= 1<<SegmentIndex::Top;
	const int	TopRight	= 1<<SegmentIndex::TopRight;
	const int	BottomRight	= 1<<SegmentIndex::BottomRight;
	const int	Bottom		= 1<<SegmentIndex::Bottom;
	const int	BottomLeft	= 1<<SegmentIndex::BottomLeft;
	const int	TopLeft		= 1<<SegmentIndex::TopLeft;
	const int	Middle		= 1<<SegmentIndex::Middle;
	
	const int	TopHalf		= Top|TopRight|Middle|TopLeft;
	const int	BottomHalf	= BottomRight|Bottom|BottomLeft|Middle;
}


#define GRID_WIDTH	8
#define GRID_HEIGHT	4
#define GRID_SIZE	(GRID_WIDTH*GRID_HEIGHT)
bool Grid[GRID_WIDTH * GRID_HEIGHT] =
{
	1,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	1,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	1,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	1,	0,
};

#define TOP	(SegmentMask::TopHalf)
#define BOT	(SegmentMask::BottomHalf)

class DigitRef
{
public:
	DigitRef(int Device,int Number,int SegmentMask) :
		Device		( Device ),
		Number		( Number ),
		Segments	( SegmentMask )
	{
	};
	
	int	Device;
	int	Number;
	int Segments;

	bool	IncludesSegment(int SegmentIndex)
	{
		return (Segments & (1<<SegmentIndex)) != 0;
	}
};

//	layout for our upside down numbers
const DigitRef Grid_To_Digit[GRID_WIDTH*GRID_HEIGHT] =
{
	DigitRef(0,7,TOP),	DigitRef(0,6,TOP),	DigitRef(0,5,TOP),	DigitRef(0,4,TOP),	DigitRef(0,3,TOP),	DigitRef(0,2,TOP),	DigitRef(0,1,TOP),	DigitRef(0,0,TOP),	
	DigitRef(0,7,BOT),	DigitRef(0,6,BOT),	DigitRef(0,5,BOT),	DigitRef(0,4,BOT),	DigitRef(0,3,BOT),	DigitRef(0,2,BOT),	DigitRef(0,1,BOT),	DigitRef(0,0,BOT),	
	DigitRef(1,0,BOT),	DigitRef(1,1,BOT),	DigitRef(1,2,BOT),	DigitRef(1,3,BOT),	DigitRef(1,4,BOT),	DigitRef(1,5,BOT),	DigitRef(1,6,BOT),	DigitRef(1,7,BOT),
	DigitRef(1,0,TOP),	DigitRef(1,1,TOP),	DigitRef(1,2,TOP),	DigitRef(1,3,TOP),	DigitRef(1,4,TOP),	DigitRef(1,5,TOP),	DigitRef(1,6,TOP),	DigitRef(1,7,TOP),
};


/* 
 This time we have more than one device. 
 But all of them have to be initialized 
 individually.
 */
void setup() 
{
  //we have already set the number of devices when we created the LedControl
  int devices=lc.getDeviceCount();
  //we have to init all devices in a loop
  for(int address=0;address<devices;address++) {
    /*The MAX72XX is in power-saving mode on startup*/
    lc.shutdown(address,false);
    /* Set the brightness to a medium values */
    lc.setIntensity(address,7);
    /* and clear the display */
    lc.clearDisplay(address);
  }
}

int GetGridIndex(int Gridx,int Gridy)
{
	int Index = Gridy * GRID_WIDTH;
	Index += Gridx;
	return Index;
}

void EnableSquare(int Gridx,int Gridy)
{
	int GridIndex = GetGridIndex( Gridx, Gridy );
	
	//	clear all
	for ( int d=0;	d<lc.getDeviceCount();	d++ )
		lc.clearDisplay(d);

	auto DigitRef = Grid_To_Digit[GridIndex];
	for ( auto s=0;	s<SegmentIndex::Count;	s++ )
		if ( DigitRef.IncludesSegment(s) )
			lc.setLed( DigitRef.Device, DigitRef.Number, s, true );
}

void loop() 
{
	//	keep re-setting to help whilst we fix connections
	setup();

	for ( int x=0;	x<GRID_WIDTH;	x++ )
	{
		for ( int y=0;	y<GRID_HEIGHT;	y++ )
		{
			EnableSquare( x, y );
			delay(100);
		}
	}
	/*
	//read the number cascaded devices
	int devices=lc.getDeviceCount();

	const int DeviceCount = lc.getDeviceCount();
	const int NumberCount = 8;
	const int SegmentCount = 8;
	const int TotalSegmentCount = DeviceCount * NumberCount * SegmentCount;

	for ( int d=0;	d<DeviceCount;	d++ )
	{
		for ( int n=0;	n<NumberCount;	n++ )
		{
			for ( int s=0;	s<SegmentCount;	s++ )
			{
				delay(delaytime);
    			lc.setLed( d, n, s, true);
				delay(delaytime);
				//lc.setLed( d, n, s, false );

			}
		}
	}
	
	/*

  //	loop each thiong
  //	we have to init all devices in a loop
      for(int address=0;address<devices;address++) 
  {
    for(int col=0;col<8;col++) 
    {
  for(int row=0;row<8;row++) 
      {
        delay(delaytime);
        lc.setLed(address,row,col,true);
        delay(delaytime);
        lc.setLed(address,row,col,false);
      }
    }
  }
  */
}
