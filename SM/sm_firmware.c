/*
*Function:
*Programed by:Ray_DK@163.com
*Complete date:
*Modified by:
*Modified date:
*Version:
*Remarks:
*/
#include "sm_firmware.h"
#include "delay.h"
#include "planner.h"
#include "stepper.h"
#include "temperature.h"
#include "motion_control.h"
#include "watchdog.h"
#include "ConfigurationStore.h"
#include "language.h"
#include "usart.h"
#include "gcodeplayer.h"
#include "children_menu.h"
#include "sdio_sdcard.h"
#include "math.h"
#include "24cxx.h"
#include "sm_plus.h"

#include "bsp.h"
#include "App_Language.h"
#include "App_Global_Function.h"
#include "App_ADC.h"
#include "App_Timer.h"


#define VERSION_STRING  "1.0.0"

// look here for descriptions of gcodes: http://linuxcnc.org/handbook/gcode/g-code.html
// http://objects.reprap.org/wiki/Mendel_User_Manual:_RepRapGCodes

//Implemented Codes
//-------------------
// G0  -> G1
// G1  - Coordinated Movement X Y Z E
// G2  - CW ARC
// G3  - CCW ARC
// G4  - Dwell S<seconds> or P<milliseconds>
// G10 - retract filament according to settings of M207
// G11 - retract recover filament according to settings of M208
// G28 - Home all Axis
// G90 - Use Absolute Coordinates
// G91 - Use Relative Coordinates
// G92 - Set current position to cordinates given

//RepRap M Codes
// M0   - Unconditional stop - Wait for user to press a button on the LCD (Only if ULTRA_LCD is enabled)
// M1   - Same as M0
// M104 - Set extruder target temp
// M105 - Read current temp
// M106 - Fan on
// M107 - Fan off
// M109 - Wait for extruder current temp to reach target temp.
// M114 - Display current position

//Custom M Codes
// M15  - update flash data (font data ; icon data and so on)
// M16  - screen_adjust
// M17  - Enable/Power all stepper motors
// M18  - Disable all stepper motors; same as M84
// M20  - List SD card
// M21  - Init SD card
// M22  - Release SD card
// M23  - Select SD file (M23 filename.g)
// M24  - Start/resume SD print
// M25  - Pause SD print
// M26  - Set SD position in bytes (M26 S12345)
// M27  - Report SD print status
// M28  - Start SD write (M28 filename.g)
// M29  - Stop SD write
// M30  - Delete file from SD (M30 filename.g)
// M31  - Output time since last M109 or SD card start to serial
// M42  - Change pin status via gcode Use M42 Px Sy to set pin x to value y, when omitting Px the onboard led will be used.
// M80  - Turn on Power Supply
// M81  - Turn off Power Supply
// M82  - Set E codes absolute (default)
// M83  - Set E codes relative while in Absolute Coordinates (G90) mode
// M84  - Disable steppers until next move,
//        or use S<seconds> to specify an inactivity timeout, after which the steppers will be disabled.  S0 to disable the timeout.
// M85  - Set inactivity shutdown timer with parameter S<seconds>. To disable set zero (default)
// M92  - Set axis_steps_per_unit - same syntax as G92
// M114 - Output current position to serial port
// M115 - Capabilities string
// M117 - display message
// M119 - Output Endstop status to serial port
// M120 - 紧急停止
// M126 - Solenoid Air Valve Open (BariCUDA support by jmil)
// M127 - Solenoid Air Valve Closed (BariCUDA vent to atmospheric pressure by jmil)
// M128 - EtoP Open (BariCUDA EtoP = electricity to air pressure transducer by jmil)
// M129 - EtoP Closed (BariCUDA EtoP = electricity to air pressure transducer by jmil)
// M140 - Set bed target temp
// M190 - Wait for bed current temp to reach target temp.
// M200 - Set filament diameter
// M201 - Set max acceleration in units/s^2 for print moves (M201 X1000 Y1000)
// M202 - Set max acceleration in units/s^2 for travel moves (M202 X1000 Y1000) Unused in Marlin!!
// M203 - Set maximum feedrate that your machine can sustain (M203 X200 Y200 Z300 E10000) in mm/sec
// M204 - Set default acceleration: S normal moves T filament only moves (M204 S3000 T7000) im mm/sec^2  also sets minimum segment time in ms (B20000) to prevent buffer underruns and M20 minimum feedrate
// M205 -  advanced settings:  minimum travel speed S=while printing T=travel only,  B=minimum segment time X= maximum xy jerk, Z=maximum Z jerk, E=maximum E jerk
// M206 - set additional homeing offset
// M207 - set retract length S[positive mm] F[feedrate mm/sec] Z[additional zlift/hop]
// M208 - set recover=unretract length S[positive mm surplus to the M207 S*] F[feedrate mm/sec]
// M209 - S<1=TRUE/0=FALSE> enable automatic retract detect if the slicer did not support G10/11: every normal extrude-only move will be classified as retract depending on the direction.
// M218 - set hotend offset (in mm): T<extruder_number> X<offset_on_X> Y<offset_on_Y>
// M220 S<factor in percent>- set speed factor override percentage
// M221 S<factor in percent>- set extrude factor override percentage
// M240 - Trigger a camera to take a photograph
// M280 - set servo position absolute. P: servo index, S: angle or microseconds
// M300 - Play beepsound S<frequency Hz> P<duration ms>
// M301 - Set PID parameters P I and D
// M302 - Allow cold extrudes
// M303 - PID relay autotune S<temperature> sets the target temperature. (default target temperature = 150C)
// M304 - Set bed PID parameters P I and D
// M400 - Finish all moves
// M500 - stores paramters in EEPROM
// M501 - reads parameters from EEPROM (if you need reset them after you changed them temporarily).
// M502 - reverts to the default "factory settings".  You still need to store them in EEPROM afterwards if you want to.
// M503 - print the current settings (from memory not from eeprom)
// M540 - Use S[0|1] to enable or disable the stop SD card print on endstop hit (requires ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED)
// M600 - Pause for filament change X[pos] Y[pos] Z[relative lift] E[initial retract] L[later retract distance for removal]
// M907 - Set digital trimpot motor current using axis codes.
// M908 - Control digital trimpot directly.
// M350 - Set microstepping mode.
// M351 - Toggle MS1 MS2 pins directly.
// M928 - Start SD logging (M928 filename.g) - ended by M29
// M999 - Restart after being stopped by error
// M4000 - 发送CPU ID
// M4001 - 发送打印参数
// M4004 - 复位mcu
// M4008 - 底板检测
// M4009 - 卸料
// M4010 - 上料
// M4101 - 复制SD卡的icon和字库到 spiflash


//Stepper Movement Variables

//===========================================================================
//=============================imported variables============================
//===========================================================================


//===========================================================================
//=============================public variables=============================
//===========================================================================
#ifdef SDSUPPORT
	CardReader card;
#endif
float homing_feedrate[] = HOMING_FEEDRATE;
bool axis_relative_modes[] = AXIS_RELATIVE_MODES;
//要求默认3倍速打印
int feedmultiply = DEFAULT_FEEDMULTIPLY,reg_feedmultiply = 100; //100->1 200->2
int saved_feedmultiply;
int extrudemultiply = 100; //100->1 200->2
float current_position[NUM_AXIS] = { 0.0, 0.0, 0.0, 0.0 };
float add_homeing[3] = {0, 0, 0};
float min_pos[3] = { X_MIN_POS, Y_MIN_POS, Z_MIN_POS };
float max_pos[3] = { X_MAX_POS, Y_MAX_POS, Z_MAX_POS };

static const float  base_min_pos[3]   = { X_MIN_POS, Y_MIN_POS, Z_MIN_POS };
static float  base_max_pos[3]   = { X_MAX_POS, Y_MAX_POS, Z_MAX_POS };
static const float  base_home_pos[3]  = { X_HOME_POS, Y_HOME_POS, Z_HOME_POS };
static float  max_length[3]	    = { X_MAX_LENGTH, Y_MAX_LENGTH, Z_MAX_LENGTH };
static const float  home_retract_mm[3] = { X_HOME_RETRACT_MM, Y_HOME_RETRACT_MM, Z_HOME_RETRACT_MM };
static const signed char home_dir[3]  = { X_HOME_DIR, Y_HOME_DIR, Z_HOME_DIR };


CMDBUFF CmdBuff = {{0}, 0};
extern USR_TIMER usr_timer;
uint8_t old_usr_timer_sec;

// Extruder offset, only in XY plane
#if EXTRUDERS > 1
float extruder_offset[2][EXTRUDERS] = {
	#if defined(EXTRUDER_OFFSET_X) && defined(EXTRUDER_OFFSET_Y)
	EXTRUDER_OFFSET_X, EXTRUDER_OFFSET_Y
	#else
	0
	#endif
};
#endif
uint8_t active_extruder = 0;
int fanSpeed = 0;
int fanSpeed1 = 100;	//喉管风扇默认值
int fanSpeed2 = 0;
#ifdef BARICUDA
	int ValvePressure = 0;
	int EtoPPressure = 0;
#endif

#ifdef FWRETRACT
	bool autoretract_enabled = TRUE;
	bool retracted = FALSE;
	float retract_length = 3, retract_feedrate = 17 * 60, retract_zlift = 0.8;
	float retract_recover_length = 0, retract_recover_feedrate = 8 * 60;
#endif
//===========================================================================
//=============================private variables=============================
//===========================================================================
const char axis_codes[NUM_AXIS] = {'X', 'Y', 'Z', 'E'};
float destination[NUM_AXIS] = {  0.0, 0.0, 0.0, 0.0};
float offset[3] = {0.0, 0.0, 0.0};
static bool home_all_axis = TRUE;
static float feedrate = 1500.0, next_feedrate, saved_feedrate;
static long gcode_N, gcode_LastN, Stopped_gcode_LastN = 0;

volatile static bool relative_mode = FALSE;  //Determines Absolute or Relative Coordinates

//static char cmdbuffer[BUFSIZE][MAX_CMD_SIZE];

volatile static bool fromsd[BUFSIZE];

int bufindr = 0;
int bufindw = 0;
int buflen = 0;
//static int i = 0;
static char serial_char;
static char serial_char2;
static int serial_count = 0;
static int serial_count2 = 0;


static bool comment_mode = FALSE;
static bool comment_mode2 = FALSE;

static char *strchr_pointer; // just a pointer to find chars in the cmd string like X, Y, Z, E, etc

//const int sensitive_pins[] = SENSITIVE_PINS; // Sensitive pin list for M42

//Inactivity shutdown variables
static unsigned long previous_millis_cmd = 0;
static unsigned long max_inactive_time = 0;
static unsigned long stepper_inactive_time = DEFAULT_STEPPER_DEACTIVE_TIME * 1000l;

unsigned long starttime = 0;
unsigned long stoptime = 0;
static u8 tmp_extruder;


bool Stopped = FALSE;
u8 SSID[40] = {'n', 'u', 'l', 'l'};	//保存SSID命令
u8 IPADDR[15] = {0};				//IP地址
extern volatile unsigned char block_buffer_head;

#ifdef LASER
u8 LaserSwitch=0;	//激光头开关标志 0 关 1开
#endif
//#if NUM_SERVOS > 0
//  Servo servos[NUM_SERVOS];
//#endif
//===========================================================================
//=============================ROUTINES=============================
//===========================================================================

void get_arc_coordinates(void);
bool setTargetedHotend(int code);
void App_PrintingRead_SD(void);




void enquecommand(const char *cmd)
{
	if(buflen < BUFSIZE) 
	{
		//this is dangerous if a mixing of serial and this happsens
		strcpy(&(CmdBuff.cmdbuffer[bufindw][0]), cmd);
		CmdBuff.type[bufindw] = 0;
		SERIAL_ECHO_START;
		USR_DbgLog("enqueing \"%s\"", CmdBuff.cmdbuffer[bufindw]);
		bufindw = (bufindw + 1) % BUFSIZE;
		buflen += 1;
	}
}


void setup(void)
{
	Config_RetrieveSettings();
	Config_PrintSettings();	//打印参数设置
	reset_acceleration_rates();
	
	//		max_pos[0] = X_MAX_POS;
	//		max_pos[1] = Y_MAX_POS;
	//		max_pos[2] = Z_MAX_POS;
	//		base_max_pos[0] = X_MAX_POS;
	//		base_max_pos[1] = Y_MAX_POS;
	//		base_max_pos[2] = Z_MAX_POS;
	//		max_length[0]	 = X_MAX_LENGTH;
	//		max_length[1]	 = Y_MAX_LENGTH;
	//		max_length[2]	 = Z_MAX_LENGTH;

	st_init();    // Initialize stepper, this enables interrupts
	tp_init();    // Initialize
	plan_init();  // Initialize planner;
	lcd_update();
}
/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:
*/
static u8 olddoorstat = 0;
void door_stat(void)
{
	u8 doorstat = 0;
	doorstat = DOOR_STAT;
	if(ChildData.door_stat != doorstat) 
	{
		if(doorstat != olddoorstat)
			olddoorstat = doorstat;
		else
			ChildData.door_stat = doorstat;
	}


}
/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:
*/
static u8 oldfilamentstat = 0;
void filament_stat(void)
{
	u8 filamentstat = 0;
	filamentstat = STOP_V;
	if(PrintInfo.filament != filamentstat)
	{
		if(filamentstat != oldfilamentstat)
			oldfilamentstat = filamentstat;
		else
			PrintInfo.filament = filamentstat;
	}
}

/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:
*/
u8 filament_lack_flg = 0;
void App_SDData_Read(void);
void App_SDGets_Data(void);
void loop(void)
{
	long heartbeat = 0;
	u8 res=0;

	while(1) 
	{

		if(heartbeat < millis()) 
		{
			heartbeat = millis() + 5000;
		}
		if(buflen < (BUFSIZE - 1)) 
		{
			get_command();
		}
		if(buflen < (BUFSIZE - 1)) 
		{
			get_command2();
		}
		#ifdef SDSUPPORT
		//if(SD_CD)
		//{
		// card.cardOK = FALSE;
		//}

		//card_checkautostart();
		#endif
		
		//App_SDData_Read();						//every time read one byte
		App_SDGets_Data();							//every time read one line
		//if(buflen > 0) 
	while(buflen>0)	
		{
			#ifdef SDSUPPORT
				if(card.saving) 
				{
					if(strstr(CmdBuff.cmdbuffer[bufindr], PSTR("M29")) == NULL) 
					{	//结束写SD卡文件
						card_write_command(CmdBuff.cmdbuffer[bufindr]);
						USR_UsrLog(MSG_OK);
					} 
					else 
					{
						card_closefile();
						USR_UsrLog(MSG_FILE_SAVED);
					}
					USR_UsrLog("\n");
				} 
				else 
				{
					process_commands();
				}
			#else
				process_commands();
			#endif //SDSUPPORT
				
			memset(CmdBuff.cmdbuffer[bufindr],0,MAX_CMD_SIZE);					
			if(buflen > 0)
			{
				buflen = (buflen - 1);
				bufindr = (bufindr + 1) % BUFSIZE;
			}
			
			
			res = blocks_full();
			if(res || (card.sdprinting==FALSE))//路径队列已满或者不在在打印状态下
				break;
		}
		//check heater every n milliseconds
		manage_heater();
		manage_inactivity();
		checkHitEndstops();
		checkDTR();//20160228
		
		lcd_update();
		
		if(usr_timer.sec != old_usr_timer_sec)		//Every second update onece;
		{
			old_usr_timer_sec = usr_timer.sec;
			printinfo_update();
		}
		
		#if MACHINE_MODE == 1
			door_stat();	//门状态检测
		#endif
		
		filament_stat();	//断料检测
		
		#if MACHINE_MODE == 2
			sd_check();		//sd卡检测
		#endif
	}
}

void get_command(void)
{
//	int16_t n;
//	char time[30];
//	unsigned long t;
//	int hours, minutes;
//	FunctionCode = FUN_1;
	while(MYSERIAL_available() > 0  && buflen < BUFSIZE) {
		serial_char = MYSERIAL_read();
		if(serial_char == '\n' || serial_char == '\r' || (serial_char == ':' && comment_mode == FALSE) || serial_count >= (MAX_CMD_SIZE - 1)) { //sanse 冒号
			if(!serial_count) { //if empty line
				comment_mode = FALSE; //for new command
				return;
			}
			CmdBuff.cmdbuffer[bufindw][serial_count] = 0; //terminate string
			if(!comment_mode) {
				comment_mode = FALSE; //for new command
				fromsd[bufindw] = FALSE;
				if(strchr(CmdBuff.cmdbuffer[bufindw], 'N') != NULL) {
					strchr_pointer = strchr(CmdBuff.cmdbuffer[bufindw], 'N');
					gcode_N = (strtol(&CmdBuff.cmdbuffer[bufindw][strchr_pointer - CmdBuff.cmdbuffer[bufindw] + 1], NULL, 10));
					if(gcode_N != gcode_LastN + 1 && (strstr(CmdBuff.cmdbuffer[bufindw], PSTR("M110")) == NULL)) 
					{
						SERIAL_ERROR_START;
						USR_UsrLog(MSG_ERR_LINE_NO);
						USR_UsrLog("%ld", gcode_LastN);
						//Serial.println(gcode_N);
						FlushSerialRequestResend();
						serial_count = 0;
						return;
					}

					if(strchr(CmdBuff.cmdbuffer[bufindw], '*') != NULL) 
					{
						u8 checksum = 0;
						u8 count = 0;
						while(CmdBuff.cmdbuffer[bufindw][count] != '*') checksum = checksum ^ CmdBuff.cmdbuffer[bufindw][count++];
						strchr_pointer = strchr(CmdBuff.cmdbuffer[bufindw], '*');

						if((u8)(strtod(&CmdBuff.cmdbuffer[bufindw][strchr_pointer - CmdBuff.cmdbuffer[bufindw] + 1], NULL)) != checksum) {
							SERIAL_ERROR_START;
							USR_UsrLog(MSG_ERR_CHECKSUM_MISMATCH);
							USR_DbgLog(" checksum: %d\n\r", checksum);
							count = 0;

							while(CmdBuff.cmdbuffer[bufindw][count] != '*') 
							{
								USR_DbgLog("%c", CmdBuff.cmdbuffer[bufindw][count++]);
							}

							checksum = 0;
							count = 0;
							while(CmdBuff.cmdbuffer[bufindw][count] != '*') 
							{
								USR_DbgLog("CmdBuff.cmdbuffer:%d;", CmdBuff.cmdbuffer[bufindw][count]);
								checksum = checksum ^ CmdBuff.cmdbuffer[bufindw][count++];
								USR_DbgLog(" checksum:%d \n\r", checksum);
							}

							USR_DbgLog("%ld", gcode_LastN);
							FlushSerialRequestResend();
							serial_count = 0;
							return;
						}
						//if no errors, continue parsing
					} 
					else 
					{
						SERIAL_ERROR_START;
						USR_UsrLog(MSG_ERR_NO_CHECKSUM);
						USR_DbgLog("%ld", gcode_LastN);
						FlushSerialRequestResend();
						serial_count = 0;
						return;
					}

					gcode_LastN = gcode_N;
					//if no errors, continue parsing
				}
				else 
				{ // if we don't receive 'N' but still see '*'
					if((strchr(CmdBuff.cmdbuffer[bufindw], '*') != NULL)) 
					{
						SERIAL_ERROR_START;
						USR_UsrLog(MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM);
						USR_UsrLog("%ld", gcode_LastN);
						serial_count = 0;
						return;
					}
				}
				if((strchr(CmdBuff.cmdbuffer[bufindw], 'G') != NULL)) 
				{
					strchr_pointer = strchr(CmdBuff.cmdbuffer[bufindw], 'G');
					switch((int)((strtod(&CmdBuff.cmdbuffer[bufindw][strchr_pointer - CmdBuff.cmdbuffer[bufindw] + 1], NULL)))) {
						case 0:
							break;
						case 1:
							break;
						case 2:
							break;
						case 3:
							if(Stopped == FALSE) 
							{ // If printer is stopped by an error the G[0-3] codes are ignored.
								#ifdef SDSUPPORT
								if(card.saving)
									break;
								#endif //SDSUPPORT
							}
							else 
							{
								USR_ErrLog(MSG_ERR_STOPPED);
								// LCD_MESSAGEPGM(MSG_STOPPED);
							}
							break;
						default:
							break;
					}
				}
				USR_UsrLog("usart1 cmd %s\r\n", CmdBuff.cmdbuffer[bufindw]);
				//memcpy(CmdBuff.type[bufindw],0,MAX_CMD_SIZE);
				CmdBuff.type[bufindw] = 0;
				bufindw = (bufindw + 1) % BUFSIZE;
				buflen += 1;//sanse

			}
			serial_count = 0; //clear buffer
		} 
		else 
		{
			if(serial_char == ';') comment_mode = TRUE;
			if(!comment_mode) CmdBuff.cmdbuffer[bufindw][serial_count++] = serial_char;
		}
	}
}

/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:Read Printing Data Frome TF Card
*/
extern FIL File;
void App_SDGets_Data(void)
{
	char time[30];
	unsigned long t;
	int hours, minutes;
	char RdBuffer[MAX_CMD_SIZE],i = 0;
	char *str;
	
	if(!card.sdprinting) 
	{
		return;
	}
	while(!card_eof() && buflen < BUFSIZE - 2)
	{		
		str = f_gets(RdBuffer,MAX_CMD_SIZE,&card.fgcode);
		if(str != NULL)
		{
			i = 0;
			if(RdBuffer[0] != ';')
			{
				if(RdBuffer[0] == 'M' || RdBuffer[0] == 'G')
				{
					for(i = 0;i < MAX_CMD_SIZE;i++)
					{
						if((RdBuffer[i] == 0x20) &&(( RdBuffer[i+1] == 0x20)|| (RdBuffer[i+1] == ';')))
						{
							break;
						}
						if(RdBuffer[i] == 0x0D||RdBuffer[i] == 0x0A )
						{
							break;
						}
					}
				}
			}
			if(i != 0)
			{
				//CmdBuff.cmdbuffer[bufindw][serial_count] = 0; //terminate string
				CmdBuff.type[bufindw] = 0;
				memset(CmdBuff.cmdbuffer[bufindw],0,MAX_CMD_SIZE);			//Clear Data Reg
				memcpy(CmdBuff.cmdbuffer[bufindw],RdBuffer,i);					//copy print data to data reg;
				buflen += 1;
				bufindw = (bufindw + 1) % BUFSIZE;
			}
		}
		if(card_eof())
		{
			PrintInfo.printsd = 0;
			USR_UsrLog(MSG_FILE_PRINTED);
			stoptime = millis();
			t = (stoptime - starttime) / 1000;
			minutes = (t / 60) % 60;
			hours = t / 60 / 60;
			sprintf(time, PSTR("%i hours %i minutes"), hours, minutes);
			SERIAL_ECHO_START;
			USR_UsrLog("%s", time);
			card_printingHasFinished();
			f_close(&card.fgcode);
		}
	}
}
/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:Read Printing Data Frome TF Card
*/
void App_SDData_Read(void)
{
	int16_t n;
	char time[30];
	unsigned long t;
	int hours, minutes;

	if(!card.sdprinting || serial_count != 0) 
	{
		return;
	}
	while(!card_eof()  && buflen < (BUFSIZE - 2)) 
	{
		n = card_get();
		if(n == -1) 
		{
			printf("***read sd card failed****\r\n");
			printf("readchar=%d\r\n", ReadChar);
		}
		serial_char = (BYTE)n;
		if(serial_char == '\n' ||
		   serial_char == '\r' ||
		   (serial_char == ':' && comment_mode == FALSE) ||
		   serial_count >= (MAX_CMD_SIZE - 1) ||
		   n == -1) 
		{
			if(card_eof()) 
			{ //sanse
				PrintInfo.printsd = 0;
				USR_UsrLog(MSG_FILE_PRINTED);
				stoptime = millis();
				t = (stoptime - starttime) / 1000;
				minutes = (t / 60) % 60;
				hours = t / 60 / 60;
				sprintf(time, PSTR("%i hours %i minutes"), hours, minutes);
				SERIAL_ECHO_START;
				USR_UsrLog("%s", time);
				//   lcd_setstatus(time);
				card_printingHasFinished();
				// card_checkautostart(TRUE);

			}
			if(!serial_count) 
			{
				comment_mode = FALSE; //for new command
				//return; //if empty line
				continue;
			}
			CmdBuff.cmdbuffer[bufindw][serial_count] = 0; //terminate string

			CmdBuff.type[bufindw] = 0;
			fromsd[bufindw] = TRUE;//sanse
			buflen += 1;
			bufindw = (bufindw + 1) % BUFSIZE;

			comment_mode = FALSE; //for new command
			serial_count = 0; //clear buffer
		} 
		else 
		{
			if(serial_char == ';')
			{
				comment_mode = TRUE;
			}
			if(!comment_mode) 
			{
				CmdBuff.cmdbuffer[bufindw][serial_count++] = serial_char;
			}
		}
	}
}

void get_command2(void)
{
	FunctionCode = FUN_2;
	while(MYSERIAL_available2() > 0	&& buflen < BUFSIZE) {
		serial_char2 = MYSERIAL_read2();
		if(serial_char2 == '\n' || serial_char2 == '\r' || (serial_char2 == ':' && comment_mode2 == FALSE) || serial_count2 >= (MAX_CMD_SIZE - 1)) { //sanse 冒号
			if(!serial_count2) { //if empty line
				comment_mode2 = FALSE; //for new command
				return;
			}
			CmdBuff.cmdbuffer[bufindw][serial_count2] = 0; //terminate string
			if(!comment_mode2) {
				comment_mode2 = FALSE; //for new command
				fromsd[bufindw] = FALSE;
				if((strchr(CmdBuff.cmdbuffer[bufindw], 'G') != NULL)) {
					strchr_pointer = strchr(CmdBuff.cmdbuffer[bufindw], 'G');
					switch((int)((strtod(&CmdBuff.cmdbuffer[bufindw][strchr_pointer - CmdBuff.cmdbuffer[bufindw] + 1], NULL)))) {
						case 0:
							break;
						case 1:
							break;
						case 2:
							break;
						case 3:
							if(Stopped == FALSE) 
							{ // If printer is stopped by an error the G[0-3] codes are ignored.
								#ifdef SDSUPPORT
								if(card.saving)
									break;
								#endif //SDSUPPORT
							} 
							else 
							{
								USR_UsrLog(MSG_ERR_STOPPED);
								// LCD_MESSAGEPGM(MSG_STOPPED);
							}
							break;
						default:
							break;
					}
				}
				USR_ErrLog("usart2 cmd %s\r\n",CmdBuff.cmdbuffer[bufindw]);
				CmdBuff.type[bufindw] = 1;
				bufindw = (bufindw + 1) % BUFSIZE;
				buflen += 1;//sanse
			}
			serial_count2 = 0; //clear buffer
		} 
		else 
		{
			if(serial_char2 == ';')
			{
				comment_mode2 = TRUE;
			}
			if(!comment_mode2)
			{
				CmdBuff.cmdbuffer[bufindw][serial_count2++] = serial_char2;
			}
		}
	}

}


float code_value(void)
{
	float res;
	res = strtod(&CmdBuff.cmdbuffer[bufindr][strchr_pointer - CmdBuff.cmdbuffer[bufindr] + 1], NULL);
	return res;
}

long code_value_long(void)
{
	return (strtol(&CmdBuff.cmdbuffer[bufindr][strchr_pointer - CmdBuff.cmdbuffer[bufindr] + 1], NULL, 10));
}

bool code_seen(char code)
{
	strchr_pointer = strchr(CmdBuff.cmdbuffer[bufindr], code);
	return (strchr_pointer != NULL);  //Return True if a character was found
}


void axis_is_at_home(int axis)
{
	current_position[axis] = base_home_pos[axis] + add_homeing[axis];
	min_pos[axis] =          base_min_pos[axis] + add_homeing[axis];
	max_pos[axis] =          base_max_pos[axis] + add_homeing[axis];
}

#define HOMEAXIS_DO(LETTER) (( LETTER##_HOME_DIR==-1) || (LETTER##_HOME_DIR==1))
static void homeaxis(int axis)
{
	if(axis == X_AXIS ? HOMEAXIS_DO(X) :
	   axis == Y_AXIS ? HOMEAXIS_DO(Y) :
	   axis == Z_AXIS ? HOMEAXIS_DO(Z) :
	   0) {	//
		current_position[axis] = 0;
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		destination[axis] = 1.5 * max_length[axis] * home_dir[axis];
		feedrate = homing_feedrate[axis];
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
		st_synchronize();

		current_position[axis] = 0;
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		destination[axis] = -home_retract_mm[axis] * home_dir[axis];
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
		st_synchronize();

		destination[axis] = 2 * home_retract_mm[axis] * home_dir[axis];
		feedrate = homing_feedrate[axis] / 2 ;
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
		st_synchronize();

		axis_is_at_home(axis);
		destination[axis] = current_position[axis];
		feedrate = 0.0;
		endstops_hit_on_purpose();
	}
}
#define HOMEAXIS(LETTER) homeaxis(LETTER##_AXIS)
#ifdef REPRINTSUPPORT
	int printline = 0;	//当前打印行数
	u8 GoToLineFlg = FALSE;
#endif

#ifdef MESH_BED_LEVELING
u8 mbl_active = 0;	//网格底板调平完成标志 TRUE 完成 FALSE 未完成
float matrix_offset[3][3] = {0.0};

float BL_TOUCH_OFFSET = 0.9;
#define BLT_XMIN	20
//#define BLT_XMIN 60
#define BLT_XLEN	77.5
#define BLT_YMIN	20
//#define BLT_YMIN	60

#define BLT_YLEN	72.5
extern volatile long endstops_trigsteps[3];
u8 mbl_flg = 0;	//自动调平工作标志  1：在自动调平





float mbl_get_z(float x, float y)
{
	int   i, j;
	float s, t;

#define MESH_MID_X (0.5f*(MESH_MIN_X+MESH_MAX_X))
#define MESH_MID_Y (0.5f*(MESH_MIN_Y+MESH_MAX_Y))
	if(x < MESH_MID_X) 
	{
		i = 0;
		s = (x - MESH_MIN_X) / MESH_MID_X;
		if(s > 1.f)
			s = 1.f;
	} 
	else 
	{
		i = 1;
		s = (x - MESH_MID_X) / MESH_MID_X;
		if(s < 0)
			s = 0;
	}
	if(y < MESH_MID_Y) 
	{
		j = 0;
		t = (y - MESH_MIN_Y) / MESH_MID_Y;
		if(t > 1.f)
			t = 1.f;
	} 
	else 
	{
		j = 1;
		t = (y - MESH_MID_Y) / MESH_MID_Y;
		if(t < 0)
			t = 0;
	}

	float si = 1.f - s;
	float z0 = si * matrix_offset[j	][i] + s * matrix_offset[j][i + 1];
	float z1 = si * matrix_offset[j + 1][i] + s * matrix_offset[j + 1][i + 1];
	return (1.f - t) * z0 + t * z1;

}

void mesh_plan_buffer_line(const float x, const float y, const float z, const float e, const float feed_rate, const uint8_t extruder)
{
	float dx = x - current_position[X_AXIS];
	float dy = y - current_position[Y_AXIS];
	float dz = z - current_position[Z_AXIS];
	int n_segments = 0;

	if(mbl_active) 
	{
		float len = abs(dx) + abs(dy);
		if(len > 0)
		{
			// Split to 3cm segments or shorter.
			n_segments = (int)(ceil(len / 30.f));
		}
	}

	if(n_segments > 1) 
	{
		float de = e - current_position[E_AXIS];
		for(int i = 1; i < n_segments; ++ i) 
		{
			float t = (float)(i) / (float)(n_segments);
			// if ((card.sdprinting == FALSE) || (mbl_active == FALSE)) return;
			if((card.sdprinting == FALSE) || (mbl_active == FALSE)) goto mbl_not_active;
			
			plan_buffer_line(
			    current_position[X_AXIS] + t * dx,
			    current_position[Y_AXIS] + t * dy,
			    current_position[Z_AXIS] + t * dz,
			    current_position[E_AXIS] + t * de,
			    feed_rate, extruder);
		}
	}
mbl_not_active:
	// The rest of the path.
	plan_buffer_line(x, y, z, e, feed_rate, extruder);
	current_position[X_AXIS] = x;
	current_position[Y_AXIS] = y;
	current_position[Z_AXIS] = z;
	current_position[E_AXIS] = e;
}

#define OFFSET_RANG 15.f


u8 mbl_process(void)
{
	u8 i, j, k;
	float temp_f;
	enable_endstops(FALSE);
	for(j = 0; j < 3; j++) {	//第一排
		//底板抬高10mm

		destination[Z_AXIS] = 10.f;
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], 3000, 0);
		current_position[Z_AXIS] = destination[Z_AXIS];
		st_synchronize();

		//移动到对应位置
		destination[X_AXIS] = BLT_XMIN + j * BLT_XLEN;
		destination[Y_AXIS] = BLT_YMIN;
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], 3000, 0);
		current_position[X_AXIS] = destination[X_AXIS];
		current_position[Y_AXIS] = destination[Y_AXIS];
		st_synchronize();
		temp_f = 0.f;
		
		#if 1
		for(k = 0; k < 2; k++) 
		{	//采集2次
			//bl_touch下伸
			BL_TOUCH_PIN = BL_DOWN;

			//Z轴下移
			destination[Z_AXIS] = -10.f;
			plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], 1200, 0);
			current_position[Z_AXIS] = destination[Z_AXIS];
			st_synchronize();
			BL_TOUCH_PIN = BL_UP;
			//得到底板偏移值
			temp_f += (float)endstops_trigsteps[Z_AXIS] / axis_steps_per_unit[Z_AXIS];
			if((temp_f > OFFSET_RANG) || (temp_f < -OFFSET_RANG)) {
				goto exit;
			}
			destination[Z_AXIS] = 8.f;
			plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], 1200, 0);
			current_position[Z_AXIS] = destination[Z_AXIS];
			st_synchronize();
		}
		#endif
		
		matrix_offset[0][j] = temp_f / 2.f;
		matrix_offset[0][j] -= BL_TOUCH_OFFSET;
		printf("matrix_offset[0][%d]=%f\r\n", j, matrix_offset[0][j]);

	}
	j = 3;
	while(j) 
	{		//第二排
		j--;
		//底板抬高10mm
		destination[Z_AXIS] = 10.f;
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], 3000, 0);
		current_position[Z_AXIS] = destination[Z_AXIS];
		st_synchronize();

		//移动到对应位置
		destination[X_AXIS] = BLT_XMIN + j * BLT_XLEN;
		destination[Y_AXIS] = BLT_YMIN + BLT_YLEN;
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], 3000, 0);
		current_position[X_AXIS] = destination[X_AXIS];
		current_position[Y_AXIS] = destination[Y_AXIS];
		st_synchronize();
		temp_f = 0.f;
		#if 1
		for(k = 0; k < 2; k++) 
		{	//采集2次
			//bl_touch下伸
			BL_TOUCH_PIN = BL_DOWN;

			//Z轴下移
			destination[Z_AXIS] = -10.f;
			plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], 1200, 0);
			current_position[Z_AXIS] = destination[Z_AXIS];
			st_synchronize();
			BL_TOUCH_PIN = BL_UP;
			//得到底板偏移值
			temp_f += (float)endstops_trigsteps[Z_AXIS] / axis_steps_per_unit[Z_AXIS];
			if((temp_f > OFFSET_RANG) || (temp_f < -OFFSET_RANG)) {
				goto exit;
			}
			destination[Z_AXIS] = 8.f;
			plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], 1200, 0);
			current_position[Z_AXIS] = destination[Z_AXIS];
			st_synchronize();
		}
		#endif
		matrix_offset[1][j] = temp_f / 2.f;
		matrix_offset[1][j] -= BL_TOUCH_OFFSET;
		printf("matrix_offset[1][%d]=%f\r\n", j, matrix_offset[1][j]);

	}
	for(j = 0; j < 3; j++) 
	{	//第三排
		//底板抬高10mm
		destination[Z_AXIS] = 10.f;
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], 3000, 0);
		current_position[Z_AXIS] = destination[Z_AXIS];
		st_synchronize();

		//移动到对应位置
		destination[X_AXIS] = BLT_XMIN + j * BLT_XLEN;
		destination[Y_AXIS] = BLT_YMIN + 2 * BLT_YLEN;
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], 3000, 0);
		current_position[X_AXIS] = destination[X_AXIS];
		current_position[Y_AXIS] = destination[Y_AXIS];
		st_synchronize();
		temp_f = 0.f;
		#if 1
		for(k = 0; k < 2; k++) {	//采集2次
			//bl_touch下伸
			BL_TOUCH_PIN= BL_DOWN;

			//Z轴下移
			destination[Z_AXIS] = -10.f;
			plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], 1200, 0);
			current_position[Z_AXIS] = destination[Z_AXIS];
			st_synchronize();
			BL_TOUCH_PIN = BL_UP;
			//得到底板偏移值
			temp_f += (float)endstops_trigsteps[Z_AXIS] / axis_steps_per_unit[Z_AXIS];
			if((temp_f > OFFSET_RANG) || (temp_f < -OFFSET_RANG)) {
				goto exit;
			}
			destination[Z_AXIS] = 8.f;
			plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], 1200, 0);
			current_position[Z_AXIS] = destination[Z_AXIS];
			st_synchronize();
		}
		#endif
		matrix_offset[2][j] = temp_f / 2.f;
		matrix_offset[2][j] -= BL_TOUCH_OFFSET;
		printf("matrix_offset[2][%d]=%f\r\n", j, matrix_offset[2][j]);

	}
	enable_endstops(FALSE);
	//得到所有采集点的Z轴偏移值后结束采集工作
	BL_TOUCH_PIN = 23;		//解除警报
	//保存所有采集点
	for(i = 0; i < 9; i++) {
		AT24CXX_Write(MBL_VALUE, (u8 *)matrix_offset, sizeof(matrix_offset));
	}
	return 0;		//正常执行完成回复0
exit:
	printf("temp_f=%2.2f\r\n", temp_f);
	return 1;

}


#endif  // MESH_BED_LEVELING


/*实现G28命令axis 低三位从低到高分别表示xyz轴需要的归零*/
void G28_process(u8 axis)
{
	u8 i;
	saved_feedrate = feedrate;
	saved_feedmultiply = feedmultiply;
	feedmultiply = 100;
	previous_millis_cmd = millis();

	enable_endstops(TRUE);

	for(i = 0; i < NUM_AXIS; i++) 
	{
		destination[i] = current_position[i];
	}
	feedrate = 0.0;

	if((axis == 0) || (axis & 0x01)) 
	{
		HOMEAXIS(X);
	}

	if((axis == 0) || (axis & 0x02)) 
	{
		HOMEAXIS(Y);
	}

	if((axis == 0) || (axis & 0x04)) 
	{
		HOMEAXIS(Z);

	}
	plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
	#ifdef ENDSTOPS_ONLY_FOR_HOMING
	enable_endstops(FALSE);
	#endif
	feedrate = saved_feedrate;
	feedmultiply = saved_feedmultiply;
	previous_millis_cmd = millis();
	endstops_hit_on_purpose();
}

void process_commands(void)
{
	unsigned long codenum; //throw away variable
	char *starpos = NULL;
	int8_t i,j;
	
	if(code_seen('G')) 
	{
		#ifdef REPRINTSUPPORT
		if(card.sdprinting == TRUE) 
		{
			printline++;
			if(GoToLineFlg == TRUE) 
			{
				if(printline != ReadPrintLine) 
				{
					return;
				} 
				else 
				{
					GoToLineFlg = FALSE;
					USR_UsrLog("go to break line=%d \r\n", printline);
					//清零命令缓存数组
					memset(&CmdBuff, 0, sizeof(CmdBuff));
					bufindw = 0;
					bufindr = 0;
					buflen = 0;
					memset(fatbuf, 0, sizeof(fatbuf));
					sprintf(fatbuf, "M190 S%d", target_temperature_bed);
					enquecommand(fatbuf);

					memset(fatbuf, 0, sizeof(fatbuf));
					sprintf(fatbuf, "M109 S%d", target_temperature[0]);
					enquecommand(fatbuf);
					enquecommand("G28 XY");	//归零
					return;
				}
			}
		}
		#endif
		switch((int)code_value()) 
		{
			case 0: // G0 -> G1
				if(CmdBuff.type[bufindr]) usart2_send("G0:OK\r\n");
				if(Stopped == FALSE) 
				{
					get_coordinates(); // For X Y Z E F
					prepare_move();
					return;
				}
			break;
			case 1: // G1
					if(CmdBuff.type[bufindr]) usart2_send("G1:OK\r\n");
					if(Stopped == FALSE) 
					{
						get_coordinates(); // For X Y Z E F
						prepare_move();
						return;
					}
				break;
			case 2: // G2  - CW ARC
					if(CmdBuff.type[bufindr])
						usart2_send("G2:OK\r\n");
					if(Stopped == FALSE) 
					{
						get_arc_coordinates();
						prepare_arc_move(TRUE);
						return;
					}
				break;
			case 3: // G3  - CCW ARC
					if(CmdBuff.type[bufindr])
						usart2_send("G3:OK\r\n");
					if(Stopped == FALSE) 
					{
						get_arc_coordinates();
						prepare_arc_move(FALSE);
						return;
					}
				break;
			case 4: // G4 dwell
					// LCD_MESSAGEPGM(MSG_DWELL); ///////////////////////////////////////////////////////////
					if(CmdBuff.type[bufindr])
						usart2_send("G4:OK");
					codenum = 0;
					if(code_seen('P')) codenum = code_value(); // milliseconds to wait
					if(code_seen('S')) codenum = code_value() * 1000; // seconds to wait

					st_synchronize();
					codenum += millis();  // keep track of when we started waiting
					previous_millis_cmd = millis();
					while(millis()  < codenum) 
					{
						manage_heater();
						manage_inactivity();
						lcd_update();	
					}
				break;
				#ifdef FWRETRACT	   //ONLY PARTIALLY TESTED
			case 10: // G10 retract
				if(CmdBuff.type[bufindr])
					usart2_send("G10:OK\r\n");
				if(!retracted) {
					destination[X_AXIS] = current_position[X_AXIS];
					destination[Y_AXIS] = current_position[Y_AXIS];
					destination[Z_AXIS] = current_position[Z_AXIS];
					current_position[Z_AXIS] += -retract_zlift;
					destination[E_AXIS] = current_position[E_AXIS] - retract_length;
					feedrate = retract_feedrate;
					retracted = TRUE;
					prepare_move();
				}
				break;
			case 11: // G10 retract_recover
				if(CmdBuff.type[bufindr])
					usart2_send("G11:OK\r\n");
				if(!retracted) {
					destination[X_AXIS] = current_position[X_AXIS];
					destination[Y_AXIS] = current_position[Y_AXIS];
					destination[Z_AXIS] = current_position[Z_AXIS];

					current_position[Z_AXIS] += retract_zlift;
					current_position[E_AXIS] += -retract_recover_length;
					feedrate = retract_recover_feedrate;
					retracted = FALSE;
					prepare_move();
					
				}
				break;
				#endif //FWRETRACT   //ONLY PARTIALLY TESTED

			case 28: //G28 Home all Axis one at a time
				if(CmdBuff.type[bufindr])
					usart2_send("G28:OK\r\n");
				#ifdef MESH_BED_LEVELING
				mbl_active = 0;
				#endif
				saved_feedrate = feedrate;
				saved_feedmultiply = feedmultiply;
				feedmultiply = 100;
				previous_millis_cmd = millis();

				enable_endstops(TRUE);

				for(i = 0; i < NUM_AXIS; i++) 
				{
					destination[i] = current_position[i];
				}
				feedrate = 0.0;
				home_all_axis = !((code_seen(axis_codes[0])) || (code_seen(axis_codes[1])) || (code_seen(axis_codes[2])));

				#if Z_HOME_DIR > 0                      // If homing away from BED do Z first
				if((home_all_axis) || (code_seen(axis_codes[Z_AXIS]))) {
					HOMEAXIS(Z);
				}
				#endif

				#ifdef QUICK_HOME
				if((home_all_axis) || (code_seen(axis_codes[X_AXIS]) && code_seen(axis_codes[Y_AXIS]))) { //first diagonal move
					current_position[X_AXIS] = 0;
					current_position[Y_AXIS] = 0;

					plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
					destination[X_AXIS] = 1.5 * X_MAX_LENGTH * X_HOME_DIR;
					destination[Y_AXIS] = 1.5 * Y_MAX_LENGTH * Y_HOME_DIR;
					feedrate = homing_feedrate[X_AXIS];
					if(homing_feedrate[Y_AXIS] < feedrate)
						feedrate = homing_feedrate[Y_AXIS];
					plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
					st_synchronize();

					axis_is_at_home(X_AXIS);
					axis_is_at_home(Y_AXIS);
					plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
					destination[X_AXIS] = current_position[X_AXIS];
					destination[Y_AXIS] = current_position[Y_AXIS];
					plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
					feedrate = 0.0;
					st_synchronize();
					endstops_hit_on_purpose();
				}
				#endif

				if((home_all_axis) || (code_seen(axis_codes[X_AXIS]))) 
				{
					HOMEAXIS(X);
				}

				if((home_all_axis) || (code_seen(axis_codes[Y_AXIS]))) 
				{
					HOMEAXIS(Y);
				}

				#if Z_HOME_DIR < 0                      // If homing towards BED do Z last
				if((home_all_axis) || (code_seen(axis_codes[Z_AXIS]))) 
				{
					HOMEAXIS(Z);

				}
				#endif

				if(code_seen(axis_codes[X_AXIS])) 
				{
					if(code_value_long() != 0) 
					{
						current_position[X_AXIS] = code_value() + add_homeing[0];
					}
				}

				if(code_seen(axis_codes[Y_AXIS])) 
				{
					if(code_value_long() != 0) 
					{
						current_position[Y_AXIS] = code_value() + add_homeing[1];
					}
				}

				if(code_seen(axis_codes[Z_AXIS])) 
				{
					if(code_value_long() != 0) 
					{
						current_position[Z_AXIS] = code_value() + add_homeing[2];
					}
				}
				plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);

				#ifdef ENDSTOPS_ONLY_FOR_HOMING
				enable_endstops(FALSE);
				#endif

				feedrate = saved_feedrate;
				feedmultiply = saved_feedmultiply;
				previous_millis_cmd = millis();
				endstops_hit_on_purpose();
				#ifdef MESH_BED_LEVELING
				if(code_seen('L') && home_all_axis) 
				{	//归零的时候使能了自动调平
					mbl_flg = TRUE;
					mbl_active = FALSE;	//清零状态

					if(mbl_process() != 0) 
					{
						mbl_active = TRUE;	//
					}
					BL_TOUCH_PIN = 23;		//解除警报
					mbl_flg = FALSE;
					mbl_active = TRUE;
				}
				#endif
				break;
			case 90: // G90
					if(CmdBuff.type[bufindr])
					{
						usart2_send("G90:OK\r\n");
					}
					relative_mode = FALSE;
				break;
			case 91: // G91
					if(CmdBuff.type[bufindr])
					{
						usart2_send("G91:OK\r\n");
					}
					relative_mode = TRUE;
					if(buflen > 3)
					{
						for(i = 0;i < NUM_AXIS;i++)
						{
							current_position[i] = 0;
						}
						plan_set_position(current_position[X_AXIS],current_position[Y_AXIS],current_position[Z_AXIS],current_position[E_AXIS]);
					}
					USR_DbgLog("relative_mode = TRUE;");

				break;
			case 92: // G92
				if(CmdBuff.type[bufindr])
				{
					usart2_send("G92:OK\r\n");
				}
				if(!code_seen(axis_codes[E_AXIS]))
					st_synchronize();
				for(i = 0; i < NUM_AXIS; i++) 
				{
					if(code_seen(axis_codes[i])) 
					{
						if(i == E_AXIS) 
						{//X_HEAD
							current_position[i] = code_value();
							plan_set_e_position(current_position[E_AXIS]);
						}
						else 
						{
							current_position[i] = code_value() + add_homeing[i];
							plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
						}
					}
				}
				break;
		}
	} 
		else if(code_seen('M')) 
		{
		#ifdef REPRINTSUPPORT
		if(card.sdprinting == TRUE) 
		{
			printline++;
			if(GoToLineFlg == TRUE) 
			{
				if(printline != ReadPrintLine) 
				{
					return;
				} 
				else 
				{
					GoToLineFlg = FALSE;
					USR_UsrLog("go to break line=%d \r\n", printline);
					//清零命令缓存数组
					memset(&CmdBuff, 0, sizeof(CmdBuff));
					bufindw = 0;
					bufindr = 0;
					buflen = 0;
					memset(fatbuf, 0, sizeof(fatbuf));
					sprintf(fatbuf, "M190 S%d", target_temperature_bed);
					enquecommand(fatbuf);

					memset(fatbuf, 0, sizeof(fatbuf));
					sprintf(fatbuf, "M109 S%d", target_temperature[0]);
					enquecommand(fatbuf);
					enquecommand("G28 XY");	//归零
					return;
				}
			}
		}

		#endif
		switch((int)code_value()) {


			case 0: // M0 - Unconditional stop - Wait for user button press on LCD
				if(CmdBuff.type[bufindr]) usart2_send("M0:OK\r\n");
				break;
			case 1: // M1 - Conditional stop - Wait for user button press on LCD
				if(CmdBuff.type[bufindr]) usart2_send("M1:OK\r\n");
				{
					#ifdef ULTIPANEL
					// LCD_MESSAGEPGM(MSG_USERWAIT);	/////////////////////////////////////////////////
					codenum = 0;
					if(code_seen('P')) codenum = code_value(); // milliseconds to wait
					if(code_seen('S')) codenum = code_value() * 1000; // seconds to wait

					st_synchronize();
					previous_millis_cmd = millis();
					if(codenum > 0) 
					{
						codenum += millis();  // keep track of when we started waiting
						while(millis()  < codenum && !LCD_CLICKED) 
						{
							manage_heater();
							manage_inactivity();
							lcd_update();	 ///////////////////////////////////
						}
					} 
					else 
					{
						while(!LCD_CLICKED) 
						{
							manage_heater();
							manage_inactivity();
							lcd_update();	 ////////////////////////////////////////
						}
					}
					//   LCD_MESSAGEPGM(MSG_RESUMING); ////////////////////////////////
					#endif
				}
				break;
#ifdef LASER
			case 3:	//M3 开激光
				LaserSwitch = 1;
				plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
			break;
			case 5:	//M5 关激光
				LaserSwitch = 0;
				plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
			break;
#endif
			case 15:// M15  - update flash data (font data ; icon data and so on)
				//	LCD_Clear(WHITE);
				//	if(update_font(5,0,12,0)==0)
				//	{	LCD_ShowString(5,80,200,200,12, "UPDATE SUCCESS!");
				//		delay_ms(500);
				//	}
				if(CmdBuff.type[bufindr]) usart2_send("M15:OK\r\n");
				break;
			case 16:// M16  - screen_adjust
				if(CmdBuff.type[bufindr]) 
					usart2_send("M16:OK\r\n");
				break;
			case 17:
				// LCD_MESSAGEPGM(MSG_NO_MOVE);	////////////////////////////////
				if(CmdBuff.type[bufindr]) 
					usart2_send("M17:OK\r\n");
				enable_x();
				enable_y();
				enable_z();
				enable_e0();
				enable_e1();
				break;
				#ifdef SDSUPPORT
			case 19:
				if(CmdBuff.type[bufindr]) 
					usart2_send("M19:OK\r\n");
				if((card.sdprinting == FALSE) && (PrintInfo.printsd == 2)) 
				{
					card.sdprinting = TRUE;
					PrintInfo.printsd = 1;
					sendFlg = 1;
				}
				break;
			case 20:
					if(CmdBuff.type[bufindr]) 
						usart2_send("M20:OK\r\n");
					USR_UsrLog(MSG_BEGIN_FILE_LIST);
					card_ls();
					USR_UsrLog(MSG_END_FILE_LIST);
				break;
			case 21:
					if(CmdBuff.type[bufindr]) 
						usart2_send("M21:OK\r\n");
					card_initsd();
				break;
			case 22:
					if(CmdBuff.type[bufindr]) 
						usart2_send("M22:OK\r\n");
					card_release();
				break;
			case 23:
				starpos = (strchr(strchr_pointer + 4, '*'));
				if(starpos != NULL)
					*(starpos - 1) = '\0';

				i=card_openFile(strchr_pointer + 4, TRUE);
				if(i)//打开失败
				{
					if(CmdBuff.type[bufindr]) usart2_send("M23:FAIL\r\n");
				}
				else
				{
					if(CmdBuff.type[bufindr]) usart2_send("M23:OK\r\n");
				}
				break;
			case 24:
				if(CmdBuff.type[bufindr]) usart2_send("M24:OK\r\n");
				card_startFileprint();
				sendFlg = 1;
				#if MACHINE_MODE==1
					CurrentMenu = print_menu;
				#elif MACHINE_MODE==2
					CurrentMenu = print_diy;
				#endif
				redraw_menu = TRUE;
				starttime = millis();
				break;
			case 25:
					if(CmdBuff.type[bufindr]) usart2_send("M25:OK\r\n");
					card_pauseSDPrint();
					PrintInfo.printsd = 2;	//暂停打印
					sendFlg = 1;
				break;
			case 26:
					if(CmdBuff.type[bufindr])
					{
						usart2_send("M26:OK\r\n");
					}
					if(card.cardOK && code_seen('S')) 
					{
						card_setIndex(code_value_long());
					}
				break;
			case 27:
				if(CmdBuff.type[bufindr]) usart2_send("M27:OK\r\n");
				card_getStatus();
				break;
			case 28:
				if(CmdBuff.type[bufindr]) usart2_send("M28:OK\r\n");
				starpos = (strchr(strchr_pointer + 4, '*'));
				if(starpos != NULL) {
					char *npos = strchr(CmdBuff.cmdbuffer[bufindr], 'N');
					strchr_pointer = strchr(npos, ' ') + 1;
					*(starpos - 1) = '\0';
				}
				card_openFile(strchr_pointer + 4, FALSE);
				break;
			case 29:
				if(CmdBuff.type[bufindr]) 
					usart2_send("M29:OK\r\n");

				break;
			case 30:
				if(CmdBuff.type[bufindr]) 
					usart2_send("M30:OK\r\n");
				if(card.cardOK) 
				{
					card_closefile();
					starpos = (strchr(strchr_pointer + 4, '*'));
					if(starpos != NULL) 
					{
						char *npos = strchr(CmdBuff.cmdbuffer[bufindr], 'N');
						strchr_pointer = strchr(npos, ' ') + 1;
						*(starpos - 1) = '\0';
					}
					card_removeFile(strchr_pointer + 4);
				}
				break;
				#endif //SDSUPPORT
			case 31:
				if(CmdBuff.type[bufindr]) usart2_send("M31:OK\r\n");
				{
					// char time[30];
					unsigned long t = (stoptime - starttime) / 1000;
					stoptime = millis();
					USR_DbgLog("%d min, %d sec", t / 60, t % sec);
					SERIAL_ECHO_START;
					USR_DbgLog("%s",time);
					autotempShutdown();
				}
				break;
			case 42:
				if(CmdBuff.type[bufindr])
				{
					usart2_send("M42:OK\r\n");
				}
				break;
			case 104: // M104
				if(CmdBuff.type[bufindr])
				{
					usart2_send("M104:OK\r\n");
				}
				if(setTargetedHotend(104)) 
				{
					break;
				}
				if(code_seen('S')) 
				{
					setTargetHotend(code_value(), tmp_extruder);
				}
				setWatch();
				break;
			case 140:
				if(CmdBuff.type[bufindr]) usart2_send("M140:OK\r\n");
				if(code_seen('S'))
				{
					setTargetBed(code_value());
				}
				break;

			case 105 :
				if(CmdBuff.type[bufindr]) usart2_send("M105:OK\r\n");
				if(setTargetedHotend(105)) 
				{
					break;
				}
				#if defined(TEMP_0_PIN)
				printf("ok T:%.1f /%.1f", degHotend(tmp_extruder), degTargetHotend(tmp_extruder));

				#if defined(TEMP_BED_PIN)
				printf(" B:%.1f /%.1f", degBed(), degTargetBed());

				#endif //TEMP_BED_PIN
				#else
				SERIAL_ERROR_START;
				printf(MSG_ERR_NO_THERMISTORS);
				#endif

				//  SERIAL_PROTOCOLPGM(" @:");
				USR_UsrLog(" @:%d", getHeaterPower(tmp_extruder));
				USR_UsrLog(" B@:%d\n", getHeaterPower(-1));

				return;
			//  break;
			case 109:
				if(CmdBuff.type[bufindr]) usart2_send("M109:OK\r\n");
				{
					bool target_direction;
					long residencyStart;
					if(setTargetedHotend(109))break;	//判断挤出头个数是否超出设定范围

					#ifdef AUTOTEMP
						autotemp_enabled = FALSE;
					#endif
					
					if(code_seen('S')) 
					{
						float res;
						res = code_value();
						setTargetHotend(res, tmp_extruder);
						#ifdef REPRINTSUPPORT
							//保存挤出头目标温度
							if(card.sdprinting == TRUE) 
							{
								AT24CXX_Write(E_EXTRUDERTEMPERATURE, (u8 *)&target_temperature[tmp_extruder], sizeof(target_temperature[tmp_extruder]));
								USR_UsrLog("save sd card print extrder temperature\r\n");
							}
						#endif	//REPRINTSUPPORT
					}
					#ifdef AUTOTEMP
						if(code_seen('S')) autotemp_min = code_value();
						if(code_seen('B')) autotemp_max = code_value();
						if(code_seen('F')) 
						{
							autotemp_factor = code_value();
							autotemp_enabled = TRUE;
						}
					#endif	//AUTOTEMP
					setWatch();
					codenum = millis();
					/* See if we are heating up or cooling down */
					target_direction = isHeatingHotend(tmp_extruder); // TRUE if heating, FALSE if cooling
					#ifdef TEMP_RESIDENCY_TIME
					residencyStart = -1;
							
					/*温度加热时,停止风扇或者风扇速度降低*/
					fanSpeed1 = 30;
						
					/* continue to loop until we have reached the target temp
					  _and_ until TEMP_RESIDENCY_TIME hasn't passed since we reached it */
					while((residencyStart == -1) || (residencyStart >= 0 && (((unsigned int)(millis() - residencyStart)) < (TEMP_RESIDENCY_TIME * 1000UL)))) 
					{				//打印温度等待
						if((millis() - codenum) > 1000UL) //1000
						{
							//Print Temp Reading and remaining time every 1 second while heating up/cooling down
							USR_UsrLog("T:%.1f E:%d", degHotend(tmp_extruder), tmp_extruder);

							#ifdef TEMP_RESIDENCY_TIME
								USR_UsrLog(" W:");
								if(residencyStart > -1) 
								{
									codenum = ((TEMP_RESIDENCY_TIME * 1000UL) - (millis() - residencyStart)) / 1000UL;
									USR_UsrLog("%ld\n", codenum);
								} 
								else 
								{
									USR_UsrLog("?\n");
								}
							#else
								USR_UsrLog("\n");
							#endif
								
							codenum = millis();
								
							if(target_temperature[tmp_extruder] < 20)			//目标温度小于20直接退出
								break;
							
						}//end if millis();
						manage_heater();
						manage_inactivity();
						lcd_update();
						
						#ifdef TEMP_RESIDENCY_TIME
						/* start/restart the TEMP_RESIDENCY_TIME timer whenever we reach target temp for the first time
						  or when current temp falls outside the hysteresis after target temp was reached */
						if((residencyStart == -1 &&  target_direction && (degHotend(tmp_extruder) >= (degTargetHotend(tmp_extruder) - TEMP_WINDOW))) ||
						   (residencyStart == -1 && !target_direction && (degHotend(tmp_extruder) <= (degTargetHotend(tmp_extruder) + TEMP_WINDOW))) ||
						   (residencyStart > -1 && labs(degHotend(tmp_extruder) - degTargetHotend(tmp_extruder)) > TEMP_HYSTERESIS)) 
						{
							residencyStart = millis();
						}
						#endif //TEMP_RESIDENCY_TIME
						
					}//=============打印温度等待
					#else
					while(target_direction ? (isHeatingHotend(tmp_extruder)) : (isCoolingHotend(tmp_extruder) && (CooldownNoWait == FALSE))) {
						if((millis() - codenum) > 1000UL) {
							//Print Temp Reading and remaining time every 1 second while heating up/cooling down
							printf("T:%.1f E:%d", degHotend(tmp_extruder), tmp_extruder);

							#ifdef TEMP_RESIDENCY_TIME
							printf(" W:");
							if(residencyStart > -1) {
								codenum = ((TEMP_RESIDENCY_TIME * 1000UL) - (millis() - residencyStart)) / 1000UL;
								printf("%ld\n", codenum);
							} else {
								printf("?\n");
							}
							#else
							printf("\n");
							#endif
							codenum = millis();
						}
						manage_heater();
						manage_inactivity();
						lcd_update();////////////////////////////////////////////////
						#ifdef TEMP_RESIDENCY_TIME
						/* start/restart the TEMP_RESIDENCY_TIME timer whenever we reach target temp for the first time
						  or when current temp falls outside the hysteresis after target temp was reached */
						if((residencyStart == -1 &&  target_direction && (degHotend(tmp_extruder) >= (degTargetHotend(tmp_extruder) - TEMP_WINDOW))) ||
						   (residencyStart == -1 && !target_direction && (degHotend(tmp_extruder) <= (degTargetHotend(tmp_extruder) + TEMP_WINDOW))) ||
						   (residencyStart > -1 && labs(degHotend(tmp_extruder) - degTargetHotend(tmp_extruder)) > TEMP_HYSTERESIS)) {
							residencyStart = millis();
						}
						#endif //TEMP_RESIDENCY_TIME
					}
					#endif //TEMP_RESIDENCY_TIME
					starttime = millis();
					previous_millis_cmd = millis();
				}
				/*温度加热时,停止风扇或者风扇速度降低*/
				fanSpeed1 = 100;
				break;
			case 190: // M190 - Wait for bed heater to reach target.
				if(CmdBuff.type[bufindr]) usart2_send("M190:OK\r\n");
				#if defined(TEMP_BED_PIN)
				// LCD_MESSAGEPGM(MSG_BED_HEATING);	   //////////////////////////////////////////////////////
				if(code_seen('S')) setTargetBed(code_value());
				codenum = millis();
				#ifdef	REPRINTSUPPORT
				//save bed tempeture
				if(card.sdprinting == TRUE) 
				{
					AT24CXX_Write(E_BEDTEMPERATURE, (u8 *)&target_temperature_bed, sizeof(target_temperature_bed));
					USR_UsrLog("save sd card print bed temperature\r\n");
				}
				#endif
				while(isHeatingBed()) {
					if((millis() - codenum) > 1000) 
					{ //Print Temp Reading every 1 second while heating up.
						float tt = degHotend(active_extruder);
						USR_UsrLog("T:%.1f E:%d B:%.1f\n", tt, active_extruder, degBed());

						codenum = millis();
					}
					manage_heater();
					manage_inactivity();
					lcd_update(); /////////////////////////////////////////////////
				}
				//LCD_MESSAGEPGM(MSG_BED_DONE);	   //////////////////////////////////////////////////
				previous_millis_cmd = millis();
				#endif
				break;

			case 106: //M106 Fan On
				if(CmdBuff.type[bufindr]) usart2_send("M106:OK\r\n");
				//		   #if defined(FAN_PIN)
				if(code_seen('S'))
				{
					fanSpeed = constrain(code_value(), 0, 255);
					//	   test(fanSpeed);
					// soft_pwm[0]=fanSpeed;
				} 
				else 
				{
					fanSpeed = 255;	
					/// test(255);
					//	soft_pwm[0]=255;
				}

				//		   #endif //FAN_PIN
				break;
			case 107: //M107 Fan Off
				if(CmdBuff.type[bufindr]) usart2_send("M107:OK\r\n");
				fanSpeed = 0;	//暂时屏蔽关闭风扇功能
				//	test(0);
				//	soft_pwm[0]=0;
				break;
			case 108:
				if(CmdBuff.type[bufindr]) usart2_send("M108:OK\r\n");
				if(code_seen('S')) 
				{
					fanSpeed1 = constrain(code_value(), 0, 255);
				} 
				else 
				{
					fanSpeed1 = 255;
				}
				break;
			case 110:
				if(CmdBuff.type[bufindr]) usart2_send("M108:OK\r\n");
				if(code_seen('S')) 
				{
					fanSpeed2 = constrain(code_value(), 0, 255);
				} 
				else 
				{
					fanSpeed2 = 255;
				}
				break;
			case 126: //M126 valve open
				if(CmdBuff.type[bufindr]) usart2_send("M126:OK\r\n");
				#ifdef BARICUDA
				// PWM for HEATER_1_PIN
				#if defined(HEATER_1_PIN)
				if(code_seen('S')) 
				{
					ValvePressure = constrain(code_value(), 0, 255);
				}
				else 
				{
					ValvePressure = 255;
				}

				#endif
				#endif
				break;
			case 112:
				if(CmdBuff.type[bufindr]) usart2_send("M112:OK\r\n");
				kill();
				break;
			case 127: //M127 valve closed
				if(CmdBuff.type[bufindr]) usart2_send("M127:OK\r\n");
				#ifdef BARICUDA
				// PWM for HEATER_1_PIN
				#if defined(HEATER_1_PIN)
				ValvePressure = 0;
				#endif
				#endif
				break;
			case 128://M128 valve open
				if(CmdBuff.type[bufindr]) usart2_send("M128:OK\r\n");
					#ifdef BARICUDA
					// PWM for HEATER_1_PIN
					#if defined(HEATER_2_PIN)
						if(code_seen('S')) 
						{
							EtoPPressure = constrain(code_value(), 0, 255);
						} 
						else 
						{
							EtoPPressure = 255;
						}
						#endif
					#endif
				break;
			case 129: //M129 valve closed
				if(CmdBuff.type[bufindr]) usart2_send("M129:OK\r\n");
				#ifdef BARICUDA
				// PWM for HEATER_1_PIN
				#if defined(HEATER_2_PIN)
				EtoPPressure = 0;
				#endif
				#endif
				break;
			case 80: // M80 - ATX Power On
				if(CmdBuff.type[bufindr]) usart2_send("M80:OK\r\n");
				// SET_OUTPUT(PS_ON_PIN); //GND	////////////////////////////////////////////////////
				//  WRITE(PS_ON_PIN, PS_ON_AWAKE);////////////////////////////////////////////////////
				break;
			case 81: // M81 - ATX Power Off
				if(CmdBuff.type[bufindr]) 
					usart2_send("M81:OK\r\n");
				break;
			case 82:
				if(CmdBuff.type[bufindr]) 
					usart2_send("M82:OK\r\n");
				axis_relative_modes[3] = FALSE;
				break;
			case 83:
				if(CmdBuff.type[bufindr]) 
					usart2_send("M83:OK\r\n");
				axis_relative_modes[3] = TRUE;
				break;
			case 18: //compatibility
				if(CmdBuff.type[bufindr]) 
					usart2_send("M18:OK\r\n");
			case 84: // M84
				if(CmdBuff.type[bufindr]) 
					usart2_send("M84:OK\r\n");
				if(code_seen('S')) 
				{
					stepper_inactive_time = code_value() * 1000;
				} 
				else 
				{
					bool all_axis = !((code_seen(axis_codes[0])) || (code_seen(axis_codes[1])) || (code_seen(axis_codes[2])) || (code_seen(axis_codes[3])));
					if(all_axis) 
					{
						st_synchronize();
						disable_e0();
						disable_e1();
						//  disable_e2();
						finishAndDisableSteppers();
					} 
					else 
					{
						st_synchronize();
						if(code_seen('X')) disable_x();
						if(code_seen('Y')) disable_y();
						if(code_seen('Z')) disable_z();
						// #if ((E0_ENABLE_PIN != X_ENABLE_PIN) && (E1_ENABLE_PIN != Y_ENABLE_PIN)) // Only enable on boards that have seperate ENABLE_PINS
						if(code_seen('E')) 
						{
							disable_e0();
							disable_e1();
							//  disable_e2();
						}
						//  #endif
					}
				}
				break;
			case 85: // M85
				if(CmdBuff.type[bufindr]) usart2_send("M85:OK\r\n");
				code_seen('S');
				max_inactive_time = code_value() * 1000;
				break;
			case 92: // M92
				if(CmdBuff.type[bufindr]) usart2_send("M92:OK\r\n");
				for(i = 0; i < NUM_AXIS; i++) 
				{
					if(code_seen(axis_codes[i])) 
					{
						if(i == 3) 
						{ // E
							float value = code_value();
							if(value < 20.0) 
							{
								float factor = axis_steps_per_unit[i] / value; // increase e constants if M92 E14 is given for netfab.
								max_e_jerk *= factor;
								max_feedrate[i] *= factor;
								axis_steps_per_sqr_second[i] *= factor;
							}
							axis_steps_per_unit[i] = value;
						}
						else 
						{
							axis_steps_per_unit[i] = code_value();
						}
					}
				}
				break;
			case 115: // M115
				if(CmdBuff.type[bufindr]) usart2_send("M115:OK\r\n");
				USR_UsrLog(MSG_M115_REPORT);
				break;
			case 117: // M117 display message		/////////////////////////////////////////////////////
				if(CmdBuff.type[bufindr]) usart2_send("M117:OK\r\n");
				starpos = (strchr(strchr_pointer + 5, '*'));
				if(starpos != NULL)
					*(starpos - 1) = '\0';
				//   lcd_setstatus(strchr_pointer + 5);	 //////////////////////////////////////////////////
				break;
			case 114: // M114
				if(CmdBuff.type[bufindr]) usart2_send("M114:OK\r\n");
				USR_DbgLog("X:%f Y:%f Z:%f E:%f", current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
				USR_UsrLog(MSG_COUNT_X);
				USR_DbgLog("%f Y:%f Z:%f\n", ((float)st_get_position(X_AXIS)) / axis_steps_per_unit[X_AXIS], ((float)st_get_position(Y_AXIS)) / axis_steps_per_unit[Y_AXIS], ((float)st_get_position(Z_AXIS)) / axis_steps_per_unit[Z_AXIS]);
				break;
			case 120: // M120
				if(CmdBuff.type[bufindr]) 
					usart2_send("M120:OK\r\n");
				enable_endstops(FALSE) ;
				break;
			case 121: // M121
				if(CmdBuff.type[bufindr]) 
					usart2_send("M121:OK\r\n");
				enable_endstops(TRUE) ;
				break;
			case 119: // M119
				if(CmdBuff.type[bufindr]) 
					usart2_send("M119:OK\r\n");
				USR_UsrLog(MSG_M119_REPORT);

				#if defined(X_MIN_PIN)
					USR_UsrLog(MSG_X_MIN);
					USR_UsrLog(((X_MIN_PIN == X_ENDSTOPS_INVERTING) ? MSG_ENDSTOP_HIT : MSG_ENDSTOP_OPEN));
				#endif
			
				#if defined(X_MAX_PIN)
					USR_UsrLog(MSG_X_MAX);
					USR_UsrLog(((X_MAX_PIN == X_ENDSTOPS_INVERTING) ? MSG_ENDSTOP_HIT : MSG_ENDSTOP_OPEN));
				#endif
			
				#if defined(Y_MIN_PIN)
					USR_UsrLog(MSG_Y_MIN);
					USR_UsrLog(((Y_MIN_PIN ^ Y_ENDSTOPS_INVERTING) ? MSG_ENDSTOP_HIT : MSG_ENDSTOP_OPEN));
				#endif
			
				#if defined(Y_MAX_PIN)
				USR_UsrLog(MSG_Y_MAX);
				USR_UsrLog(((Y_MAX_PIN ^ Y_ENDSTOPS_INVERTING) ? MSG_ENDSTOP_HIT : MSG_ENDSTOP_OPEN));
				#endif
				
				#if defined(Z_MIN_PIN)
				USR_UsrLog(MSG_Z_MIN);
				USR_UsrLog(((Z_MIN_PIN ^ Z_ENDSTOPS_INVERTING) ? MSG_ENDSTOP_HIT : MSG_ENDSTOP_OPEN));
				#endif
				
				#if defined(Z_MAX_PIN)
				USR_UsrLog(MSG_Z_MAX);
				USR_UsrLog(((Z_MAX_PIN ^ Z_ENDSTOPS_INVERTING) ? MSG_ENDSTOP_HIT : MSG_ENDSTOP_OPEN));
				#endif
				
				break;
			//TODO: update for all axis, use for loop
			case 201: // M201
				if(CmdBuff.type[bufindr]) 
					usart2_send("M201:OK\r\n");
				for(i = 0; i < NUM_AXIS; i++) 
				{
					if(code_seen(axis_codes[i])) 
					{
						max_acceleration_units_per_sq_second[i] = code_value();
					}
				}
				// steps per sq second need to be updated to agree with the units per sq second (as they are what is used in the planner)
				reset_acceleration_rates();
				break;
			case 202: // M202
				if(CmdBuff.type[bufindr]) 
					usart2_send("M202:OK\r\n");
				#if 0 // Not used for Sprinter/grbl gen6
				for(i = 0; i < NUM_AXIS; i++) {
					if(code_seen(axis_codes[i])) axis_travel_steps_per_sqr_second[i] = code_value() * axis_steps_per_unit[i];
				}
				#endif
				break;
			case 203: // M203 max feedrate mm/sec
				if(CmdBuff.type[bufindr]) 
					usart2_send("M203:OK\r\n");
				for(i = 0; i < NUM_AXIS; i++) 
				{
					if(code_seen(axis_codes[i])) max_feedrate[i] = code_value();
				}
				break;
			case 204: // M204 acclereration S normal moves T filmanent only moves
				if(CmdBuff.type[bufindr]) 
					usart2_send("M204:OK\r\n");
				{
					if(code_seen('S')) acceleration = code_value() ;
					if(code_seen('T')) retract_acceleration = code_value() ;
				}
				break;
			case 205: //M205 advanced settings:  minimum travel speed S=while printing T=travel only,  B=minimum segment time X= maximum xy jerk, Z=maximum Z jerk
				if(CmdBuff.type[bufindr]) 
					usart2_send("M205:OK\r\n");
				{
					if(code_seen('S')) minimumfeedrate = code_value();
					if(code_seen('T')) mintravelfeedrate = code_value();
					if(code_seen('B')) minsegmenttime = code_value() ;
					//if(code_seen('X')) max_xy_jerk = code_value() ;
					if(code_seen('X')) max_xy_jerk = min(max_xy_jerk,code_value());
					if(code_seen('Z')) max_z_jerk = code_value() ;
					if(code_seen('E')) max_e_jerk = code_value() ;
				}
				break;
			case 206: // M206 additional homeing offset
				if(CmdBuff.type[bufindr]) 
					usart2_send("M206:OK\r\n");
				for(i = 0; i < 3; i++) 
				{
					if(code_seen(axis_codes[i])) add_homeing[i] = code_value();
				}
				break;
				#ifdef FWRETRACT
			case 207: //M207 - set retract length S[positive mm] F[feedrate mm/sec] Z[additional zlift/hop]
				if(CmdBuff.type[bufindr]) usart2_send("M207:OK\r\n");
				{
					if(code_seen('S')) {
						retract_length = code_value() ;
					}
					if(code_seen('F')) {
						retract_feedrate = code_value() ;
					}
					if(code_seen('Z')) {
						retract_zlift = code_value() ;
					}
				}
				break;
			case 208: // M208 - set retract recover length S[positive mm surplus to the M207 S*] F[feedrate mm/sec]
				if(CmdBuff.type[bufindr]) usart2_send("M208:OK\r\n");
				{
					if(code_seen('S')) {
						retract_recover_length = code_value() ;
					}
					if(code_seen('F')) {
						retract_recover_feedrate = code_value() ;
					}
				}
				break;
			case 209: // M209 - S<1=TRUE/0=FALSE> enable automatic retract detect if the slicer did not support G10/11: every normal extrude-only move will be classified as retract depending on the direction.
				if(CmdBuff.type[bufindr]) usart2_send("M209:OK\r\n");
				{
					if(code_seen('S')) {
						int t = code_value() ;
						switch(t) {
							case 0:
								autoretract_enabled = FALSE;
								retracted = FALSE;
								break;
							case 1:
								autoretract_enabled = TRUE;
								retracted = FALSE;
								break;
							default:
								SERIAL_ECHO_START;
								printf(MSG_UNKNOWN_COMMAND);
								printf("%d", cmdbuffer[bufindr]);
								printf("\"");
						}
					}

				}
				break;
				#endif // FWRETRACT
				#if EXTRUDERS > 1
			case 218: // M218 - set hotend offset (in mm), T<extruder_number> X<offset_on_X> Y<offset_on_Y>
				if(CmdBuff.type[bufindr]) usart2_send("M218:OK\r\n");
				{
					if(setTargetedHotend(218)) {
						break;
					}
					if(code_seen('X')) {
						extruder_offset[X_AXIS][tmp_extruder] = code_value();
					}
					if(code_seen('Y')) {
						extruder_offset[Y_AXIS][tmp_extruder] = code_value();
					}
					SERIAL_ECHO_START;
					printf(MSG_HOTEND_OFFSET);
					for(tmp_extruder = 0; tmp_extruder < EXTRUDERS; tmp_extruder++) {
						//SERIAL_ECHO(" ");
						printf(" %f,%f", extruder_offset[X_AXIS][tmp_extruder], extruder_offset[Y_AXIS][tmp_extruder]);
						// SERIAL_ECHO(",");
						//  SERIAL_ECHO(extruder_offset[Y_AXIS][tmp_extruder]);
					}
					printf("\n");
				}
				break;
				#endif
			case 220: // M220 S<factor in percent>- set speed factor override percentage
				if(CmdBuff.type[bufindr]) usart2_send("M220:OK\r\n");
				{
					if(code_seen('S')) 
					{
						feedmultiply = code_value() ;
					}
					if(feedmultiply > DEFAULT_MAX_FEEDMULTIPLY)								//feedmultiply not over default feed
					{
						feedmultiply = DEFAULT_MAX_FEEDMULTIPLY;
					}
				}
				break;
			case 221: // M221 S<factor in percent>- set extrude factor override percentage

			{
				if(code_seen('S')) 
				{
					extrudemultiply = code_value() ;
				}
			}
			break;
			#if NUM_SERVOS > 0
			case 280: // M280 - set servo position absolute. P: servo index, S: angle or microseconds
				if(CmdBuff.type[bufindr]) usart2_send("M280:OK\r\n");
				{
					int servo_index = -1;
					int servo_position = 0;
					if(code_seen('P'))
						servo_index = code_value();
					if(code_seen('S')) {
						servo_position = code_value();
						if((servo_index >= 0) && (servo_index < NUM_SERVOS)) {
							servos[servo_index].write(servo_position);
						} else {

						}
					} else if(servo_index >= 0) {

					}
				}
				break;
				#endif // NUM_SERVOS > 0
			// #if LARGE_FLASH == TRUE && ( BEEPER > 0 || defined(ULTRALCD) )
			case 300: // M300
				if(CmdBuff.type[bufindr]) usart2_send("M300:OK\r\n");
				{
					//  int beepS = 400;
					int beepP = 1000;
					//if(code_seen('S')) beepS = code_value();
					if(code_seen('P')) beepP = code_value();

					delay_ms(beepP);
				}
				break;
				//   #endif // M300
				#ifdef PIDTEMP
			case 301: // M301
				if(CmdBuff.type[bufindr]) usart2_send("M301:OK\r\n");
				{
					if(code_seen('P')) Kp = code_value();
					if(code_seen('I')) Ki = scalePID_i(code_value());
					if(code_seen('D')) Kd = scalePID_d(code_value());

					#ifdef PID_ADD_EXTRUSION_RATE
					if(code_seen('C')) Kc = code_value();
					#endif

					updatePID();
					printf(MSG_OK);
					printf(" p:%f i:%f d:%f", Kp, unscalePID_i(Ki), unscalePID_d(Kd));

					#ifdef PID_ADD_EXTRUSION_RATE

					//Kc does not have scaling applied above, or in resetting defaults
					printf(" c:%f", Kc);
					//  SERIAL_PROTOCOL(Kc);
					#endif
					printf("\n");
				}
				break;
				#endif //PIDTEMP
				#ifdef PIDTEMPBED
			case 304: // M304
				if(CmdBuff.type[bufindr]) usart2_send("M304:OK\r\n");
				{
					if(code_seen('P')) bedKp = code_value();
					if(code_seen('I')) bedKi = scalePID_i(code_value());
					if(code_seen('D')) bedKd = scalePID_d(code_value());

					updatePID();
					printf(MSG_OK);
					printf(" p:%f i:%f d:%f", Kp, unscalePID_i(bedKi, unscalePID_d(bedKd)));
					printf("\n");

				}
				break;
				#endif //PIDTEMP
			case 240: // M240  Triggers a camera by emulating a Canon RC-1 : http://www.doc-diy.net/photo/rc-1_hacked/
				if(CmdBuff.type[bufindr]) usart2_send("M240:OK\r\n");
				{
					#if defined(PHOTOGRAPH_PIN)

					#endif
				}
				break;
			case 302: // allow cold extrudes
				if(CmdBuff.type[bufindr]) usart2_send("M302:OK\r\n");
				{
					allow_cold_extrudes(TRUE);
				}
				break;
			case 303: // M303 PID autotune
				if(CmdBuff.type[bufindr]) usart2_send("M303:OK\r\n");
				{
					float temp = 150.0;
					int e = 0;
					int c = 5;
					if(code_seen('E')) e = code_value();
					if(e < 0)
						temp = 70;
					if(code_seen('S')) temp = code_value();
					if(code_seen('C')) c = code_value();
					PID_autotune(temp, e, c);
				}
				break;
			case 400: // M400 finish all moves
				if(CmdBuff.type[bufindr]) usart2_send("M400:OK\r\n");
				{
					st_synchronize();
				}
				break;
			case 500: // M500 Store settings in EEPROM
				if(CmdBuff.type[bufindr]) usart2_send("M500:OK\r\n");
				{
					Config_StoreSettings();////////////////////////////////////////////////////////////////////////////////////////
				}
				break;
			case 501: // M501 Read settings from EEPROM
				if(CmdBuff.type[bufindr]) usart2_send("M501:OK\r\n");
				{
					Config_RetrieveSettings();////////////////////////////////////////////////////////////////////////////////////
				}
				break;
			case 502: // M502 Revert to default settings
				if(CmdBuff.type[bufindr]) usart2_send("M502:OK\r\n");
				{
					//		        Config_ResetDefault();//////////////////////////////////////////////////////////////////////////////////////////
				}
				break;
			case 503: // M503 print settings currently in memory
				if(CmdBuff.type[bufindr]) usart2_send("M503:OK\r\n");
				{
					Config_PrintSettings();///////////////////////////////////////////////////////////////////////////////////////////
				}
				break;
				#ifdef ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED
			case 540:
				if(CmdBuff.type[bufindr]) usart2_send("M540:OK\r\n");
				{
					if(code_seen('S')) abort_on_endstop_hit = code_value() > 0;
				}
				break;
				#endif

				#ifdef FILAMENTCHANGEENABLE
			case 600: //Pause for filament change X[pos] Y[pos] Z[relative lift] E[initial retract] L[later retract distance for removal]
				if(CmdBuff.type[bufindr]) usart2_send("M600:OK\r\n");
				{
					float target[4];
					float lastpos[4];
					target[X_AXIS] = current_position[X_AXIS];
					target[Y_AXIS] = current_position[Y_AXIS];
					target[Z_AXIS] = current_position[Z_AXIS];
					target[E_AXIS] = current_position[E_AXIS];
					lastpos[X_AXIS] = current_position[X_AXIS];
					lastpos[Y_AXIS] = current_position[Y_AXIS];
					lastpos[Z_AXIS] = current_position[Z_AXIS];
					lastpos[E_AXIS] = current_position[E_AXIS];
					//retract by E
					if(code_seen('E')) {
						target[E_AXIS] += code_value();
					} else {
						#ifdef FILAMENTCHANGE_FIRSTRETRACT
						target[E_AXIS] += FILAMENTCHANGE_FIRSTRETRACT ;
						#endif
					}
					plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], feedrate / 60, active_extruder);

					//lift Z
					if(code_seen('Z')) {
						target[Z_AXIS] += code_value();
					} else {
						#ifdef FILAMENTCHANGE_ZADD
						target[Z_AXIS] += FILAMENTCHANGE_ZADD ;
						#endif
					}
					plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], feedrate / 60, active_extruder);

					//move xy
					if(code_seen('X')) {
						target[X_AXIS] += code_value();
					} else {
						#ifdef FILAMENTCHANGE_XPOS
						target[X_AXIS] = FILAMENTCHANGE_XPOS ;
						#endif
					}
					if(code_seen('Y')) {
						target[Y_AXIS] = code_value();
					} else {
						#ifdef FILAMENTCHANGE_YPOS
						target[Y_AXIS] = FILAMENTCHANGE_YPOS ;
						#endif
					}

					plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], feedrate / 60, active_extruder);

					if(code_seen('L')) {
						target[E_AXIS] += code_value();
					} else {
						#ifdef FILAMENTCHANGE_FINALRETRACT
						target[E_AXIS] += FILAMENTCHANGE_FINALRETRACT ;
						#endif
					}

					plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], feedrate / 60, active_extruder);

					//finish moves
					st_synchronize();
					//disable extruder steppers so filament can be removed
					disable_e0();
					disable_e1();
					disable_e2();
					delay_ms(100);

					cnt = 0;
					while(!lcd_clicked) {
						cnt++;
						manage_heater();
						manage_inactivity();
						lcd_update();
						if(cnt == 0) {
							beep();
						}
					}

					//return to normal
					if(code_seen('L')) {
						target[E_AXIS] += -code_value();
					} else {
						#ifdef FILAMENTCHANGE_FINALRETRACT
						target[E_AXIS] += (-1) * FILAMENTCHANGE_FINALRETRACT ;
						#endif
					}
					current_position[E_AXIS] = target[E_AXIS]; //the long retract of L is compensated by manual filament feeding
					plan_set_e_position(current_position[E_AXIS]);
					plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], feedrate / 60, active_extruder); //should do nothing
					plan_buffer_line(lastpos[X_AXIS], lastpos[Y_AXIS], target[Z_AXIS], target[E_AXIS], feedrate / 60, active_extruder); //move xy back
					plan_buffer_line(lastpos[X_AXIS], lastpos[Y_AXIS], lastpos[Z_AXIS], target[E_AXIS], feedrate / 60, active_extruder); //move z back
					plan_buffer_line(lastpos[X_AXIS], lastpos[Y_AXIS], lastpos[Z_AXIS], lastpos[E_AXIS], feedrate / 60, active_extruder); //final untretract
				}
				break;
				#endif
			case 907: // M907 Set digital trimpot motor current using axis codes.
				if(CmdBuff.type[bufindr]) usart2_send("M907:OK\r\n");
				{
					for(i = 0; i < NUM_AXIS; i++) if(code_seen(axis_codes[i])) digipot_current(i, code_value());
					if(code_seen('B'))
					{
						digipot_current(4, code_value());
					}
					if(code_seen('S'))
					{
						for(i = 0; i <= 4; i++) digipot_current(i, code_value());
					}
				}
				break;
			case 908: // M908 Control digital trimpot directly.
				if(CmdBuff.type[bufindr]) usart2_send("M908:OK\r\n");
				{
					uint8_t channel, current;
					if(code_seen('P')) channel = code_value();
					if(code_seen('S')) current = code_value();
					digipot_current(channel, current);
				}
				break;
			case 350: // M350 Set microstepping mode. Warning: Steps per unit remains unchanged. S code sets stepping mode for all drivers.
				if(CmdBuff.type[bufindr]) usart2_send("M350:OK\r\n");
				{
					if(code_seen('S')) for(i = 0; i <= 4; i++) microstep_mode(i, code_value());
					for(i = 0; i < NUM_AXIS; i++)
					{					
						if(code_seen(axis_codes[i]))
						{
							microstep_mode(i, (uint8_t)code_value());
						}
					}
					if(code_seen('B'))
					{
						microstep_mode(4, code_value());
					}
					microstep_readings();
				}
				break;
			case 351: // M351 Toggle MS1 MS2 pins directly, S# determines MS1 or MS2, X# sets the pin high/low.
				if(CmdBuff.type[bufindr]) usart2_send("M351:OK\r\n");
				{
					if(code_seen('S'))
					{
						switch((int)code_value()) 
						{
							case 1:
								for(i = 0; i < NUM_AXIS; i++) if(code_seen(axis_codes[i])) microstep_ms(i, code_value(), -1, -1);
								if(code_seen('B')) microstep_ms(4, code_value(), -1, -1);
								break;
							case 2:
								for(i = 0; i < NUM_AXIS; i++) if(code_seen(axis_codes[i])) microstep_ms(i, -1, code_value(), -1);
								if(code_seen('B')) microstep_ms(4, -1, code_value(), -1);
								break;
							case 3:
								for(i = 0; i < NUM_AXIS; i++) if(code_seen(axis_codes[i])) microstep_ms(i, -1, -1, code_value());
								if(code_seen('B')) microstep_ms(4, -1, -1, code_value());
								break;
						}
					}
					microstep_readings();

				}
				break;
			case 4000:
				memset(fatbuf, 0, sizeof(fatbuf));
				sprintf(fatbuf, "M4000:0/%08X%08X%08X/%d\r\n", CPUID[0], CPUID[1], CPUID[2],sm_version);
				if(CmdBuff.type[bufindr])
				{
					usart2_send(fatbuf);
				}
				else
				{					
					usart1_send(fatbuf);
				}
				break;
			case 4001:
					memset(fatbuf, 0, sizeof(fatbuf));
					sprintf(fatbuf, "M4001:%d/%s/%d/%02d/%d\r\n", \
									PrintInfo.printsd, PrintInfo.printfile, ReadChar, PrintInfo.filesize, PrintInfo.printtime);
					if(CmdBuff.type[bufindr])
					{
						usart2_send(fatbuf);
					}
					else
					{					
						usart1_send(fatbuf);
					}
				break;
			case 4005:
					BEEP = 1;
					delay_second(2);
					BEEP = 0;
				break;
			
			case 4008:	//M4008
				if(CmdBuff.type[bufindr])
				{
					usart2_send("M4008:OK\r\n");
				}
				//置位底板检测移动标志位
				G28_process(0x03);		//G28 XY
				//Z轴太高到200
				destination[Z_AXIS] = 200;
				plan_buffer_line(destination[X_AXIS],destination[Y_AXIS],destination[Z_AXIS],destination[E_AXIS],3000,0);
				current_position[Z_AXIS] = destination[Z_AXIS];
				st_synchronize();
				//采集9个点
				for(i=0;i<3;i++)
				{
					for(j=0;j<3;j++)
					{
						destination[X_AXIS] = 38 + j*38;
						destination[Y_AXIS] = 38 + i*38;
						plan_buffer_line(destination[X_AXIS],destination[Y_AXIS],destination[Z_AXIS],destination[E_AXIS],1000,0);
						current_position[X_AXIS] = destination[X_AXIS];
						current_position[Y_AXIS] = destination[Y_AXIS];
						st_synchronize();
						delay_ms(200);
						delay_ms(200);
					}
				}		
				usart2_send("UNSOL:M4008:OK\r\n");

				break;
			case 4009:	//M4009
				if(CmdBuff.type[bufindr])
				{
					usart2_send("M4009:OK\r\n");
				}

				//置位退料命令标志位
				FilamentUnloadFlg = 1;
				//执行退料命令
				if(code_seen(' ')) 
				{
					i = (u8)code_value();
					USR_DbgLog("i=%d\r\n", i);
					switch(i)
					{
						case 1://1号料盘 执行1号料盘退料操作
							enquecommand("G91");
							enquecommand("G1 E-500 F1200");
							enquecommand("G90");
							break;
						case 2:
							break;
						case 3:
							break;
					}
				}
				break;
			case 4010:	//M4010
				if(CmdBuff.type[bufindr]) usart2_send("M4010:OK\r\n");
				//置位上料命令标志位
				FilamentLoadFlg = 1;
				//执行上料命令
				if(code_seen(' ')) 
				{
					i = (u8)code_value();
					switch(i)
					{
						case 1://1号料盘 执行1号料盘退料操作
							enquecommand("G91");
							enquecommand("G1 E505 F1200");
							enquecommand("G90");
							break;
						case 2:
							break;
						case 3:
							break;
					}
				}
				break;
			case 4011:		//wifi状态连接
				if(code_seen(' ')) 
				{
					i = (u8)code_value();
					switch(i)
					{
						case 0://未连接wifi
							ChildData.wifi_stat = 2;
							break;
						case 1://wifi连接
							ChildData.wifi_stat = 1;
							break;
						case 2:
							break;
						case 3://websocket
							ChildData.wifi_stat = 0;
							break;
					}
				}
				break;
			/*
			case 4012: //发送Z轴高度
				memset(fatbuf, 0, 30);
				AT24CXX_Read(E_G_CODE_VALUE, (u8 *)&ZAxisValue, 4);
				sprintf(fatbuf, "M4012:%f\r\n", ZAxisValue);
				usart2_send(fatbuf);
				break;
			*/
			case 4013:	//返回门状态
				memset(fatbuf, 0, 30);
				sprintf(fatbuf, "M4013:%d", ChildData.door_stat);
				if(CmdBuff.type[bufindr])
				{
					usart2_send(fatbuf);
				}
				else 
				{
					usart1_send(fatbuf);
				}
				break;
			
			case 4014:
				memset(fatbuf, 0, 30);
				sprintf(fatbuf, "M4014:%d", PrintInfo.filament);
				if(CmdBuff.type[bufindr])
				{
					usart2_send(fatbuf);
				}
				else
				{
					usart1_send(fatbuf);
				}
				break;
			case 4015:
				if(code_seen(' '))
				{
					i=(u8)code_value();
					PrintInfo.print_prepare = i;
				}
				if(PrintInfo.printsd==0)
				{
					PrintInfo.printsd = 3;
					sendFlg = 1;
				}
				if(CmdBuff.type[bufindr])
				{
					usart2_send("M4015:OK\r\n");
				}
				else
				{
					usart1_send("M4015:OK\r\n");
				}
			break;
			case 4016:
			if(code_seen(' '))
			{
				wifi_version = (u32)strtod(strchr_pointer,NULL);
				USR_DbgLog("strchr_pointer = %d value=%c*",strchr_pointer,(char)*strchr_pointer);
				//获取ssid
				strchr_pointer += 1;
				strchr_pointer = strchr(strchr_pointer,' ');
				strchr_pointer += 1;
				USR_DbgLog("strchr_pointer = %d value=%c*",strchr_pointer,(char)*strchr_pointer);
				for(i=0;i<40;i++)
				{
					if(*strchr_pointer !=' ')
						SSID[i] = *strchr_pointer;
					else
						break;
					strchr_pointer++;
				}
				//获取IP地址
				strchr_pointer += 1;
				printf("strchr_pointer = %d value=%c*",strchr_pointer,(char)*strchr_pointer);
				for(i=0;i<15;i++)
				{
					if(*strchr_pointer !='\r' || *strchr_pointer !='\n' )
					{
						IPADDR[i] = *strchr_pointer;
					}
					else
					{
						break;
					}
					strchr_pointer++;
				}
			}
			break;
			case 4017:
				PrintInfo.printsd = 0;
				PrintInfo.print_prepare = 0;
				if(CmdBuff.type[bufindr])
				{
					usart2_send("M4017:OK\r\n");
				}
				sendFlg = 1;
			break;
			case 4020:
				if(CmdBuff.type[bufindr]) 
				{
					usart2_send("M4004:OK\r\n");
				}
				//复位mcu
				NVIC_SystemReset();
			break;
			case 4021:	//解压指定文件
				if(CmdBuff.type[bufindr]) 
				{
					usart2_send("M4021:OK\r\n");
				}
				memset(pname, 0, sizeof(pname));
				strcpy(pname, strchr_pointer);
				USR_DbgLog("receive cmd is %s\r\n", pname);
				qlzdecompressed(strchr_pointer + 6);
				break;
			case 4022:	//压缩指定文件
				if(CmdBuff.type[bufindr]) 
				{
					usart2_send("M4022:OK\r\n");
				}
				memset(pname, 0, sizeof(pname));
				strcpy(pname, strchr_pointer);
				USR_DbgLog("receive cmd is %s\r\n", pname);
				qlzcompress(strchr_pointer + 6);
				break;
			case 4023:
				#if MACHINE_MODE
				quickStop(); 
				card_closefile();
				starttime=0;
				card.sdprinting = FALSE;
				PrintInfo.printsd = 0;
				AT24CXX_WriteOneByte(E_SDPRINT,FALSE);
				autotempShutdown();
				setTargetHotend(0,active_extruder);	
				setTargetBed(0); 
				disable_x(); 
				disable_y(); 
				disable_z(); 
				disable_e0(); 
				disable_e1();
				memset(CmdBuff.cmdbuffer,0,sizeof(CmdBuff.cmdbuffer));
			
				enquecommand("G28 XY");

				redraw_menu=TRUE;
				sendFlg = TRUE;
				CurrentMenu = main_menu;
				#endif
			break;
			case 4098:				//根据需求手动格式化SPI FLASH
					App_FORMAT_SPI_FLASH();
				break;
			case 4099://复制diy机型UI到spiflash中
				copy_diy_icons();
			    ls_spi_diy_icon();
				printf("System Write PIC Source Completed...");
			break;
			case 4100:
				if(CmdBuff.type[bufindr]) usart2_send("M4101:OK\r\n");
				copy_font();
				printf("System Write HZK Source Completed...");
			break;
			case 4101:	//M4101
				if(CmdBuff.type[bufindr]) usart2_send("M4101:OK\r\n");
				copyfiles();
				ls_spi_icon();
			break;
			case 4102:	//M4102
				if(CmdBuff.type[bufindr]) usart2_send("M4102:OK");
				if(code_seen('D')) 
				{
					i = (u8)code_value();
					if(i)
						INVERT_X_DIR = TRUE;
					else
						INVERT_X_DIR = FALSE;
				}
				break;
			case 4103:	//M4103
				if(CmdBuff.type[bufindr]) usart2_send("M4103:OK");
				if(code_seen('D')) 
				{
					i = (u8)code_value();
					if(i)
						INVERT_Y_DIR = TRUE;
					else
						INVERT_Y_DIR = FALSE;
				}
				break;
			case 4104:	//M4104
				if(CmdBuff.type[bufindr]) usart2_send("M4104:OK");
				if(code_seen('D')) 
				{
					i = (u8)code_value();
					if(i)
						INVERT_Z_DIR = TRUE;
					else
						INVERT_Z_DIR = FALSE;
				}
				break;
			case 4105:	//M4105
				if(CmdBuff.type[bufindr]) usart2_send("M4105:OK");
				else usart1_send("M4105:OK");
				if(code_seen('D')) 
				{
					i = (u8)code_value();
					if(i)
						INVERT_E0_DIR = TRUE;
					else
						INVERT_E0_DIR = FALSE;
				}
				break;
			case 4106:	//M4106
				if(CmdBuff.type[bufindr]) usart2_send("M4106:OK");
				else usart1_send("M4106:OK");
				if(code_seen('D')) 
				{
					i = (u8)code_value();
					if(i)
						INVERT_E1_DIR = TRUE;
					else
						INVERT_E1_DIR = FALSE;
				}
				break;
			case 4107:	//M4107
				if(CmdBuff.type[bufindr]) usart2_send("M4107:OK");
				else usart1_send("M4107:OK");
				if(code_seen('D')) 
				{
					i = (u8)code_value();
					if(i)
						INVERT_E2_DIR = TRUE;
					else
						INVERT_E2_DIR = FALSE;
				}
				break;
			case 4108:	//M4108
				if(CmdBuff.type[bufindr]) usart2_send("M4108:OK");
				else usart2_send("M4108:OK");
				if(code_seen('D')) 
				{
					i = (u8)code_value();
					if(i)
						X_ENDSTOPS_INVERTING = 1;
					else
						X_ENDSTOPS_INVERTING = 0;
				}
				break;
			case 4109:	//M4109
				if(CmdBuff.type[bufindr]) usart2_send("M4109:OK");
				else usart2_send("M4109:OK");
				if(code_seen('D')) 
				{
					i = (u8)code_value();
					if(i)
						Y_ENDSTOPS_INVERTING = 1;
					else
						Y_ENDSTOPS_INVERTING = 0;
				}
				break;
			case 4110:	//M4110
				if(CmdBuff.type[bufindr]) usart2_send("M4110:OK");
				if(code_seen('D')) 
				{
					i = (u8)code_value();
					if(i)
						Z_ENDSTOPS_INVERTING = 1;
					else
						Z_ENDSTOPS_INVERTING = 0;
				}
				break;
			case 4111:	//M4111
				if(CmdBuff.type[bufindr]) usart2_send("M4111:OK");
				if(code_seen('D')) 
				{
					i = (u8)code_value();
					if(i)
						U_ENDSTOPS_INVERTING = 1;
					else
						U_ENDSTOPS_INVERTING = 0;
				}
				break;
			case 4112:	//M4112
				if(CmdBuff.type[bufindr]) usart2_send("M4112:OK");
				if(code_seen('D')) 
				{
					i = (u8)code_value();
					if(i)
					{
						V_ENDSTOPS_INVERTING = 1;
					}
					else
					{
						V_ENDSTOPS_INVERTING = 0;
					}
				}
				break;
			case 4113:	//M4113
				if(CmdBuff.type[bufindr]) usart2_send("M4113:OK");
				if(code_seen('D')) 
				{
					i = (u8)code_value();
					if(i)
					{
						W_ENDSTOPS_INVERTING = 1;
					}
					else
					{
						W_ENDSTOPS_INVERTING = 0;
					}
				}
				break;
			case 5001:	//M5001获取打印机设备信息
				memset(fatbuf, 0, sizeof(fatbuf));
				sprintf(fatbuf, "M5001:%03d/%03d/%03d/%01d/1", X_MAX_POS, Y_MAX_POS, Z_MAX_POS, EXTRUDERS);
				if(CmdBuff.type[bufindr])
				{
					usart2_send(fatbuf);
				}
				else
				{
					usart1_send(fatbuf);
				}
				break;
			case 5002:	//M5002 获得打印信息
				memset(fatbuf, 0, sizeof(fatbuf));
				sprintf(fatbuf, "M5002:%01d/%f/%f/%f/%f/%f/%f/%f/%03d/%d/%d/%d", \
				        PrintInfo.printsd, current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], \
				        degBed(), degTargetBed(), degHotend(0), degTargetHotend0(), \
				        fanSpeed, ReadChar, PrintInfo.filesize, PrintInfo.printtime);
				if(CmdBuff.type[bufindr]) 
				{
					usart2_send(fatbuf);
				}
				else
				{
					usart1_send(fatbuf);
				}
				break;
			case 6001:		//M6001
				#ifdef MESH_BED_LEVELING
				if(code_seen('S')) 
				{	//配置bltouch偏移量
					BL_TOUCH_OFFSET = constrain(code_value(), 0, 255);
					AT24CXX_Write(MBL_OFFSET, (u8 *)&BL_TOUCH_OFFSET, sizeof(BL_TOUCH_OFFSET));
					USR_DbgLog("set BL_TOUCH_OFFSET=%1.2f\r\n", BL_TOUCH_OFFSET);
				} 
				else 
				{			//获取当前偏移量值
					USR_DbgLog("BL_TOUCH_OFFSET=%1.2f\r\n", BL_TOUCH_OFFSET);
				}
				#endif
				break;
			case 6002:	//	下伸
				BL_TOUCH_PIN = 6;
				break;
			case 6003:	//	上缩
				BL_TOUCH_PIN = 15;
				break;
			case 6004:	//自我检测
				BL_TOUCH_PIN = 18;
				break;
			case 6005:	//解除警报
				BL_TOUCH_PIN = 23;
				break;
			case 6006:	//M5006舱门信息
				usart1_send("5006:OK");
				if(code_seen('D')) 
				{
					i = (u8)code_value();
					if(i)
					{
						ChildData.door_stat = 1;
					}
					else
					{
						ChildData.door_stat = 0;
					}
				}
				break;
			case 6007:	//M5007料盘信息
				if(code_seen('D')) 
				{
					i = (u8)code_value();
					PrintInfo.printsd = i;
				}
				break;
			case 6008:	//M5008WIFI状态
				if(code_seen('D')) 
				{
					i = (u8)code_value();
					ChildData.wifi_stat = i;
				}
				break;
			case 999: // M999: Restart after being stopped
				if(CmdBuff.type[bufindr]) usart2_send("M999:OK\r\n");
				Stopped = FALSE;
				//  lcd_reset_alert_level();//////////////////////////////////////
				gcode_LastN = Stopped_gcode_LastN;
				FlushSerialRequestResend();
				break;

		}//end switch(int) code_value();

	} 
	else if(code_seen('T')) 
	{
		tmp_extruder = code_value();
		if(tmp_extruder >= EXTRUDERS) 
		{
			SERIAL_ECHO_START;
			printf("T%d", tmp_extruder);
			// SERIAL_ECHO(tmp_extruder);
			printf(MSG_INVALID_EXTRUDER);
		} 
		else 
		{
			volatile bool make_move = FALSE;
			if(code_seen('F')) 
			{
				make_move = TRUE;
				next_feedrate = code_value();
				if(next_feedrate > 0.0) 
				{
					feedrate = next_feedrate;
				}
			}
			#if EXTRUDERS > 1
			if(tmp_extruder != active_extruder) 
			{
				// Save current position to return to after applying extruder offset
				memcpy(destination, current_position, sizeof(destination));
				// Offset extruder (only by XY)
				for(i = 0; i < 2; i++) {
					current_position[i] = current_position[i] -
					                      extruder_offset[i][active_extruder] +
					                      extruder_offset[i][tmp_extruder];
				}
				// Set the new active extruder and position
				active_extruder = tmp_extruder;
				plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
				// Move to the old position if 'F' was in the parameters
				if(make_move && Stopped == FALSE) {
					prepare_move();
				}
			}	//end   if(tmp_extruder != active_extruder)
			#endif
			SERIAL_ECHO_START;
			USR_UsrLog(MSG_ACTIVE_EXTRUDER);
			USR_DbgLog("%d", active_extruder);
			USR_DbgLog("\n");
		}
	}//end else if(code_seen('T'))
	else 
	{
		//SERIAL_ECHO_START;
		//printf(MSG_UNKNOWN_COMMAND);
		//printf("%s",cmdbuffer[bufindr]);
		//printf("\"");
	}
	ClearToSend();
}

void FlushSerialRequestResend()
{

	MYSERIAL_flush();
	USR_UsrLog(MSG_RESEND);
	USR_UsrLog("%d\n", gcode_LastN + 1);
	ClearToSend();
}

void ClearToSend()
{
	previous_millis_cmd = millis();
	#ifdef SDSUPPORT
	if(fromsd[bufindr])
		return;
	#endif //SDSUPPORT

	USR_UsrLog(MSG_OK);
}




//获得坐标值
void get_coordinates()
{
	int8_t i;
	volatile bool seen[4] = {FALSE, FALSE, FALSE, FALSE};
	//	float lenth=0.0;

	for(i = 0; i < NUM_AXIS; i++) 
	{
		if(code_seen(axis_codes[i])) 
		{
			destination[i] = (float)code_value() + (axis_relative_modes[i] || relative_mode) * current_position[i];
			
			#ifdef REPRINTSUPPORT
			
			if(GoToLineFlg == FALSE && axis_codes[i] == 'Z' && card.sdprinting == TRUE) 
			{	//在正常打印情况下保存E轴数据
				AT24CXX_Write(E_G_CODE_VALUE, (u8 *)&destination[i], 4);
				AT24CXX_Write(E_G_CODE_EAXIS, (u8 *)&destination[3], 4);	//保存E轴值
				AT24CXX_Write(E_G_CODE_LINE, (u8 *)&printline, 4);			//保存打印行数
			}
			#endif


			seen[i] = TRUE;
			//	  if(axis_codes[i]=='Z')
			//	  	olddestation = destination[i];
		} 
		else 
		{
			destination[i] = current_position[i]; //Are these else lines really needed?
		}

		//	printf("get_coordinates\r\n");
		#if 0
		lenth = fabs(destination[3] - current_position[3]);
		if(lenth > 10) 
		{
			USR_DbgLog("********E_axis current_position is %f destination is %f*********\r\n", current_position[3], destination[3]);
			USR_DbgLog("process command is %s\r\n", CmdBuff.cmdbuffer[bufindr]);
			i = (bufindr > 0) ? bufindr - 1 : 7;
			USR_DbgLog("previous command is %s\r\n", CmdBuff.cmdbuffer[i]);
		}
		#endif
		
		#ifdef DEBUG_PRINTF
			USR_DbgLog("%d：%f\n", i, destination[i]);
		#endif

	}


	if(code_seen('F'))
	{
		next_feedrate = code_value();
		if(next_feedrate > 0.0) 
		{
			feedrate = next_feedrate;
		}
	}
	
	#ifdef DEBUG_PRINTF
		USR_DbgLog("feedrate：%f\n", feedrate);
	#endif

	#ifdef FWRETRACT
	if(autoretract_enabled)
		if(!(seen[X_AXIS] || seen[Y_AXIS] || seen[Z_AXIS]) && seen[E_AXIS]) {
			float echange = destination[E_AXIS] - current_position[E_AXIS];
			if(echange < -MIN_RETRACT) { //retract
				if(!retracted) {

					destination[Z_AXIS] += retract_zlift; //not sure why chaninging current_position negatively does not work.
					//if slicer retracted by echange=-1mm and you want to retract 3mm, corrrectede=-2mm additionally
					float correctede = -echange - retract_length;
					//to generate the additional steps, not the destination is changed, but inversely the current position
					current_position[E_AXIS] += -correctede;
					feedrate = retract_feedrate;
					retracted = TRUE;
				}

			} else if(echange > MIN_RETRACT) { //retract_recover
				if(retracted) {
					//current_position[Z_AXIS]+=-retract_zlift;
					//if slicer retracted_recovered by echange=+1mm and you want to retract_recover 3mm, corrrectede=2mm additionally
					float correctede = -echange + 1 * retract_length + retract_recover_length; //total unretract=retract_length+retract_recover_length[surplus]
					current_position[E_AXIS] += correctede; //to generate the additional steps, not the destination is changed, but inversely the current position
					feedrate = retract_recover_feedrate;
					retracted = FALSE;
				}
			}

		}
	#endif //FWRETRACT
}
void get_arc_coordinates(void)
{
	#ifdef SF_ARC_FIX
	bool relative_mode_backup = relative_mode;
	relative_mode = TRUE;
	#endif
	get_coordinates();
	#ifdef SF_ARC_FIX
	relative_mode = relative_mode_backup;
	#endif

	if(code_seen('I')) 
	{
		offset[0] = code_value();
	} 
	else 
	{
		offset[0] = 0.0;
	}
	if(code_seen('J')) 
	{
		offset[1] = code_value();
	} 
	else 
	{
		offset[1] = 0.0;
	}
}
void clamp_to_software_endstops(float target[3])
{
	if(min_software_endstops) 
	{
		if(target[X_AXIS] < min_pos[X_AXIS]) target[X_AXIS] = min_pos[X_AXIS];
		if(target[Y_AXIS] < min_pos[Y_AXIS]) target[Y_AXIS] = min_pos[Y_AXIS];
		if(target[Z_AXIS] < min_pos[Z_AXIS]) target[Z_AXIS] = min_pos[Z_AXIS];
	}

	if(max_software_endstops) 
	{
		if(target[X_AXIS] > max_pos[X_AXIS]) target[X_AXIS] = max_pos[X_AXIS];
		if(target[Y_AXIS] > max_pos[Y_AXIS]) target[Y_AXIS] = max_pos[Y_AXIS];
		if(target[Z_AXIS] > max_pos[Z_AXIS]) target[Z_AXIS] = max_pos[Z_AXIS];
	}
}

void prepare_move(void)
{
	int8_t i,add_temp;
	clamp_to_software_endstops(destination);
	previous_millis_cmd = millis();
	// Do not use feedmultiply for E or Z only moves
	if((current_position[X_AXIS] == destination [X_AXIS]) && (current_position[Y_AXIS] == destination [Y_AXIS])) 
	{
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
	}
//	else if(destination[E_AXIS] == current_position[E_AXIS])			//only x and y move not e,stand for empty move,
//	{
//		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate*150 / 60, active_extruder);
//	}
	else 
	{
		#ifdef MESH_BED_LEVELING      //modified by kim 20191126
			if(current_position[Z_AXIS] < 0.5)
			{
				//mesh_plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate * 100 * (1. / (60.f * 100.f)), active_extruder);
				mesh_plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
			}
			else
			{
				mesh_plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate * feedmultiply/60/100, active_extruder);
				//mesh_plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate * (80 + feedmultiply/2)/60/100, active_extruder);
				
				/*speed acc temperature handler max add 20 add by kim 20191127*/
				if(feedmultiply != reg_feedmultiply)
				{
					if(feedmultiply < DEFAULT_MAX_FEEDMULTIPLY)						//DEFAULT_MAX_FEEDMULTIPLY = 500;
					{
						if(feedmultiply > reg_feedmultiply)
						{
							add_temp = (feedmultiply - reg_feedmultiply)/50;
							target_temperature[EXTRUDERS - 1] += add_temp;
						}
						else
						{
							add_temp = (reg_feedmultiply - feedmultiply)/50;
							target_temperature[EXTRUDERS - 1] -= add_temp;
						}
					}
					else
					{
						target_temperature[EXTRUDERS - 1] += 10;
					}
					
					reg_feedmultiply = feedmultiply;
					/*升温限制,超过240以后,在往上加速,不在自动提升温度*/
					if(target_temperature[EXTRUDERS - 1] > DEFAULT_MAX_EXTRUDER_TEMP)									//DEFAULT_CONFIG Configuration.h
					{
						target_temperature[EXTRUDERS - 1] = DEFAULT_MAX_EXTRUDER_TEMP;
					}
					
					if(target_temperature[EXTRUDERS - 1] < DEFAULT_MIN_EXTRUDER_TEMP)
					{
						target_temperature[EXTRUDERS - 1] = DEFAULT_MIN_EXTRUDER_TEMP;
					}
				}
			}
		#else
			if(current_position[Z_AXIS] < 0.5)
			{
				plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate * 100 * (1. / (60.f * 100.f)), active_extruder);
			}
			else
			{
				plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate * (80 + feedmultiply/2) * (1. / (60.f * 100.f)), active_extruder);
				/*speed acc temperature handler max add 20 add by kim 20191127*/
				if(feedmultiply != reg_feedmultiply)
				{
					reg_feedmultiply = feedmultiply;
					
					add_temp = feedmultiply/20;
					
					if(feedmultiply < DEFAULT_MAX_FEEDMULTIPLY)
					{
						target_temperature[EXTRUDERS] += add_temp;
					}
					else
					{
						target_temperature[EXTRUDERS] += 20;
					}
				}
			}
		#endif					//end of modified 20191126

	}
	for(i = 0; i < NUM_AXIS; i++) {
		current_position[i] = destination[i];
	}
}

void prepare_arc_move(u8 isclockwise)
{
	int8_t i;
	float r = hypot(offset[X_AXIS], offset[Y_AXIS]); // Compute arc radius for mc_arc

	// Trace the arc
	mc_arc(current_position, destination, offset, X_AXIS, Y_AXIS, Z_AXIS, feedrate * feedmultiply / 60 / 100.0, r, isclockwise, active_extruder);

	// As far as the parser is concerned, the position is now == target. In reality the
	// motion control system might still be processing the action and the real tool position
	// in any intermediate location.
	for(i = 0; i < NUM_AXIS; i++) {
		current_position[i] = destination[i];
	}
	previous_millis_cmd = millis();
}

void manage_inactivity(void)
{
	FunctionCode = FUN_5;
	if((millis() - previous_millis_cmd) >  max_inactive_time)
	{
		if(max_inactive_time)
		{
			kill();
		}
	}
	if(stepper_inactive_time)  
	{
		if((millis() - previous_millis_cmd) >  stepper_inactive_time) 
		{
			if(blocks_queued() == FALSE) 
			{
				disable_x();
				disable_y();
				disable_z();
				disable_e0();
				disable_e1();
				disable_e2();
			}
		}
	}
	check_axes_activity();
}

void kill(void)
{
	CRITICAL_SECTION_START; // Stop interrupts
	disable_heater();

	disable_x();
	disable_y();
	disable_z();
	disable_e0();
	disable_e1();
	disable_e2();


	SERIAL_ERROR_START;
	USR_UsrLog(MSG_ERR_KILLED);
	// LCD_ALERTMESSAGEPGM(MSG_KILLED);
	//  suicide();
	while(1) 
	{ /* Intentionally left empty */ 
		
	} // Wait for reset
}

void Stop(void)
{
	disable_heater();
	if(Stopped == FALSE) {

		Stopped = TRUE;
		Stopped_gcode_LastN = gcode_LastN; // Save last g_code for restart
		SERIAL_ERROR_START;
		USR_UsrLog(MSG_ERR_STOPPED);

	}
}

bool IsStopped(void)
{
	return Stopped;
}

/*
* 判断挤出头个数是否超出设定范围
*/
bool setTargetedHotend(int code)
{
	tmp_extruder = active_extruder;
	if(code_seen('T')) 
	{
		tmp_extruder = code_value();
		if(tmp_extruder >= EXTRUDERS) 
		{
			SERIAL_ECHO_START;
			switch(code) 
			{
				case 104:
					USR_UsrLog(MSG_M104_INVALID_EXTRUDER);
					break;
				case 105:
					USR_UsrLog(MSG_M105_INVALID_EXTRUDER);
					break;
				case 109:
					USR_UsrLog(MSG_M109_INVALID_EXTRUDER);
					break;
				case 218:
					USR_UsrLog(MSG_M218_INVALID_EXTRUDER);
					break;
			}
			USR_DbgLog("%d", tmp_extruder);
			return TRUE;
		}
	}
	return FALSE ;
}
#ifdef REPRINTSUPPORT
u32 ReadPrintLine;
u8 NeedRePrintFlg = FALSE;
extern bool windows_flag;
u8 GCodeBuffer[128] = {0};
float ZAxisValue = 0;
float readFeedRate = 0.0;
void reprintcheck(void)
{
	u8 temp_u8;
	if(AT24CXX_ReadOneByte(E_SDPRINT) == TRUE) {
		//选择是否需要续打
		reprint_menu();
		if(NeedRePrintFlg == TRUE) 
		{
			USR_DbgLog("need reprint file\r\n");
		} 
		else 
		{
			USR_DbgLog("do not need reprint file\r\n");
			return;
		}

		//强行设为自动调平模式
		#ifdef MESH_BED_LEVELING
		mbl_active = TRUE;
		#endif
		//读文件名
		temp_u8 = AT24CXX_ReadOneByte(E_FILENAME);
		AT24CXX_Read(E_FILENAME + 1, pname, temp_u8);
		USR_DbgLog("read save file name is %s\r\n", pname);
		//读底板目标温度和挤出头目标温度
		AT24CXX_Read(E_EXTRUDERTEMPERATURE, (u8 *)&target_temperature[0], sizeof(target_temperature[0]));
		USR_UsrLog("read save extruder temperature is %d\r\n", target_temperature[0]);
		AT24CXX_Read(E_BEDTEMPERATURE, (u8 *)&target_temperature_bed, sizeof(target_temperature_bed));
		USR_UsrLog("read save bed temperature is %d\r\n", target_temperature_bed);
		//		sprintf(TempBuffer, "M109 S%.1f",(float)target_temperature[0]);
		//		enquecommand(TempBuffer);
		//		sprintf(TempBuffer, "M190 S%.1f",(float)target_temperature_bed);
		//		enquecommand(TempBuffer);
		//读行号
		AT24CXX_Read(E_G_CODE_LINE, (u8 *)&ReadPrintLine, sizeof(ReadPrintLine));
		USR_DbgLog("read save sd card print line is %d\r\n", ReadPrintLine);
		//Z轴值
		AT24CXX_Read(E_G_CODE_VALUE, (u8 *)&ZAxisValue, 4);
		USR_DbgLog("read z axis value = %f", ZAxisValue);
		current_position[Z_AXIS] = ZAxisValue;
		destination[Z_AXIS] = ZAxisValue;
		//读E轴数据
		AT24CXX_Read(E_G_CODE_EAXIS, (u8 *)&current_position[3], sizeof(current_position[3]));
		destination[3] = current_position[3];
		USR_DbgLog("read e axis is %f \r\n", current_position[3]);
		//设置E轴和Z轴电机位置
		plan_set_position(0, 0, current_position[Z_AXIS], current_position[E_AXIS]);
		//读取打印速率
		AT24CXX_Read(E_G_CODE_FEED, (u8 *)&feedrate, sizeof(feedrate));
		USR_DbgLog("read feedrate is %f\r\n", feedrate);
		GoToLineFlg = TRUE;
		//选中文件
		temp_u8 = f_open(&card.fgcode, (const TCHAR *)pname, FA_READ);
		if(temp_u8 == FR_OK) 
		{
			USR_DbgLog("open gcode file successed\r\n");
			#if MACHINE_MODE == 1
			CurrentMenu = print_menu;
			#elif MACHINE_MODE == 2
			CurrentMenu = print_diy;
			#endif
			redraw_menu = TRUE;
			//windows_flag=FALSE;
			card.cardOK = TRUE;	//这里做强制sd卡初始化ok
			if(card.cardOK) 
			{
				card_startFileprint();
			} 
			else
			{
				USR_ErrLog("######card.cardOK error#######\r\n");
			}
			starttime = millis();
		}
		else
		{
			USR_ErrLog("###### open gcode file filed #########\r\n");
			USR_ErrLog("error num is 0x%02x\r\n", temp_u8);
		}


	}
}
#endif



