/****************************************************************/
/* demo.c														*/
/****************************************************************/
#include "peg.hpp"
#include "demo.h"
#include "math.h"
#include "HEADER.h"
#include <embkern.h>
#include <embclib.h>
#include <malloc.h>
#include <video.h>


#include "Header.h"
#include "tunes.inc"
#include <process.h>

unsigned int __stdcall sm_thread(void* pArguments);
unsigned int __stdcall music_thread(void* pArguments);



/****************************************************************/
/* MAIN                                                         */
/****************************************************************/

/* FUNCTION PROTOTYPE */

Ubyte KeyBeep(void);  // wait for key, then BEEP
void deductNoOfSnacks(int number[], int snackID, int no);
int promptNoOfSnacks();
bool promptConfirmSelection(bool confirm);
int promptUserAddSnackPayment();
void paymentProcessing();
int promptUserToPay();


/*  MAIN CODE BEGIN HERE */
void demo()
{
	/* VARIABLES */
	bool start = true;
	bool confirm = 0; // To store user confirm selection, 0 - Not Confirm, 1 - Confirm 
	bool secConfirm = 0;
	bool start_end = false; // if false display img_START, if true do not display img_START

	/* SNACKS VARIABLES */
	int noOfSnacks[] = { 0,10,10,10,10,10,10 };
	double snackPrice[] = { 0.0,1.0,3,2.5,0.5,1.5,2.0 }; //{NIL, Snickers, Doritos, Lays, Mentos, Haribo, Seaweed}
	int reqSnackQty = 0; // requested snack qty
	int snackCheck = 0;
	double reqSnackPrice = 0.0; // requested snack price
	double totalSnackPrice = 0.0; // final total snack price
	int selectionAttempt = 0; // User # of attempts on selections


	/* SELECTION VARIABLES */
	int selectionOneQTY = 0; // to store quantity of snack selected (first)
	int selectionTwoQTY = 0; // to store quantity of snack selected (second)
	double selectionOnePRICE = 0.0; // to store price of snack selected (first)
	double selectionTwoPRICE = 0.0; // to store price of snack selected (second)

	/* STORAGE VARIABLES */
	char temp[16];
	char dblmessage;


	HANDLE hThread_sm, hThread_music;
	unsigned int threadID_sm, threadID_music;
	PegPresentationManager *pScrMgr;
	EcsPic *pEcsPic;

	HANDLE hpEcsPic, hpScrMgr, hGuiThread;

	void EcsGuiInit(HANDLE *hGuiThread, HANDLE *hpScrMgr, HANDLE *hpEcsPic);
	char EcsDrawBMP(char *file, PegPresentationManager *pScrMgr);

	/* IMAGES DIRECTORIES */
	char img_INIT[] = "C:\\project\\W0.BMP"; // INITIATE THE MONITOR
	char img_START[] = "C:\\project\\w1.BMP"; // "PRESS 0 TO START"
	char img_SELECTSNACK1[] = "C:\\project\\w2.BMP"; // "SELECT SNACK (1-6)" FIRST SELECTION
	char img_SELECTSNACK2[] = "C:\\project\\w2_1.BMP"; // "SELECT SNACK (1-6)" SECOND SELECTION
	char img_SELECTQUANTITY[] = "C:\\project\\w3.BMP"; // "SELECT QUANTITY" UP TO 3
	char img_SNACKNOTAVAIL[] = "C:\\project\\w3_1.BMP"; // "SNACK NOT AVAILABLE"
	char img_CONFIRMSELECTION[] = "C:\\project\\w4.BMP"; // "CONFIRM SELECTION? 1 - YES  0 - NO"
	char img_SELECTIONCLEAR[] = "C:\\project\\w4_1.BMP"; // "SELECTION CLEARED!"
	char img_NEXTSNACKPAYMENT[] = "C:\\project\\w5.BMP"; // "ADD SNACK OR TO PAYMENT?"
	char img_SELECTEDTWICE[] = "C:\\project\\w5_1.BMP"; // "YOU HAVE SELECTED TWICE! 1 - TO PAYMENT"
	char img_PAYMENT[] = "C:\\project\\w6.BMP"; // "PRESS 1 TO PAY" 
	char img_PP1[] = "C:\\project\\w7_2.BMP"; // "PAYMENT PROCESSING."
	char img_PP2[] = "C:\\project\\w7_3.BMP"; // "PAYMENT PROCESSING.."
	char img_PP3[] = "C:\\project\\w7_1.BMP"; // "PAYMENT PROCESSING..."
	char img_PAYMENTSUCCESSFUL[] = "C:\\project\\w8.BMP"; // "PAYMENT SUCCESSFUL"
	char img_SNACKDISPENSE[] = "C:\\project\\w9.BMP"; // "SNACK DISPENSING"
	char img_ENJOY[] = "C:\\project\\w10.BMP"; // "ENJOY YOUR SNACK!"

	char toggleIm, k, s[10];
	LedDisp(0xc);
	LcdInit();
	BEEP();

	EcsGuiInit(&hGuiThread, &hpScrMgr, &hpEcsPic);	/* initialise graphics */
	pEcsPic = (EcsPic *)hpEcsPic;
	pScrMgr = (PegPresentationManager *)hpScrMgr;

	toggleIm = 0;

	/* hThread_sm = (HANDLE)_beginthreadex(NULL,0,&sm_thread,NULL,0,&threadID_sm);
	SuspendThread(hThread_sm);
	hThread_music = (HANDLE)_beginthreadex(NULL,0,&music_thread,NULL,0,&threadID_music);
	*/

	/* THIS IS A TEST SITE */
	//LcdClear();
	//char mem[sizeof(dummy)];
	//sprintf(mem, "%0.2f", dummy);
	//LcdLine1();LcdMsg("Test dbl:");
	//LcdLine2();LcdMsg(mem);
	//while(1);
	//LcdClear();
	//while(1){
	//	sprintf(temp,*KeyPressed());
	//	LcdLine1();LcdMsg(mem);
	//	Sleep(1000);
	//}
	//while(1)

	/* INITIALISE LCD SCREEN */
	LcdClear();
	LcdLine1(); LcdMsg("      ECS      ");
	LcdLine2(); LcdMsg("   SNACK BAR   ");

	/* INITIALISING "ECS SNACK BAR" */
	EcsDrawBMP(img_INIT, pScrMgr); // Print LCD INIT image
	Sleep(1500);

	while (1)
	{
		/* START PHASE */
		if (start_end != 1) { // if start_end is false display start screen
			EcsDrawBMP(img_START, pScrMgr); // PRESS 0 to start
			LcdClear();
			LcdLine1(); LcdMsg("      ECS      ");
			LcdLine2(); LcdMsg("   SNACK BAR   ");
			Sleep(10);
			while (KeyBeep() != 0);

			/* SELECT SNACK PHASE */

			EcsDrawBMP(img_SELECTSNACK1, pScrMgr); // PRESS 0 to start
			LcdClear();
			LcdLine1(); LcdMsg("      ECS      ");
			LcdLine2(); LcdMsg("   SNACK BAR   ");
			start_end = 1;
		}
		else {			// if start_end is true do the following
			if (selectionAttempt == 0) { // if selectionAttempt and confirm == 0
				if (confirm != 1) {
					EcsDrawBMP(img_SELECTSNACK1, pScrMgr); // PRESS 0 to start
					LcdClear();
					LcdLine1(); LcdMsg("      ECS      ");
					LcdLine2(); LcdMsg("   SNACK BAR   ");
				}
			}
			else if (selectionAttempt == 1) { // if selectionAttempt = 1 & confirm == 0
				if (confirm != 1) {
					EcsDrawBMP(img_SELECTSNACK2, pScrMgr);
					LcdClear();
					LcdLine1(); LcdMsg("      ECS      ");
					LcdLine2(); LcdMsg("   SNACK BAR   ");
				}
			}

		}


		Sleep(10);
		switch (KeyBeep())
		{
		case 1: // When user select Snicker $1

			if (noOfSnacks[1] <= 0) {
				EcsDrawBMP(img_SNACKNOTAVAIL, pScrMgr); // Display snack not available 
				itoa((int)noOfSnacks[1], s, 10);
				LcdClear();
				LcdLine1(); LcdMsg("    Snickers    ");
				LcdLine2(); LcdMsg("Qty: "); LcdMsg(s);
				Sleep(1000);
				//break; // this will break the switch case if noOfSnacks[1] < 1;
				if (selectionAttempt == 0) {
					start_end = 0;
				}
				else if (selectionAttempt == 1) {
					start_end = 1;
				}

				continue;
			}
			itoa((int)noOfSnacks[1], s, 10); // itoa ( int value, char * str, int base ), Convert integer to string (non-standard function)

			/* SELECT_QUANTITY PHASE */

			EcsDrawBMP(img_SELECTQUANTITY, pScrMgr);
			LcdClear();
			LcdLine1(); LcdMsg("    Snickers    ");
			LcdLine2(); LcdMsg("Qty: "); LcdMsg(s);
			Sleep(3000);

			reqSnackQty = promptNoOfSnacks(); // will return either 1/2/3

			snackCheck = noOfSnacks[1] - reqSnackQty; // Deduct the balance

			while ((snackCheck) < 0) { // Check if the user selection is valid 
				LcdClear();
				LcdLine1(); LcdMsg("Insuficient Qty");
				LcdLine2(); LcdMsg("Qty Left: ");
				LcdMsg(s);
				Sleep(2000);
				LcdClear();
				LcdLine1(); LcdMsg("     Please     ");
				LcdLine2(); LcdMsg("  Select Again  ");
				Sleep(2000);
				LcdClear();

				reqSnackQty = promptNoOfSnacks();
				snackCheck = noOfSnacks[1] - reqSnackQty; // Deduct the balance
				if (snackCheck > 0)
					break;

				//snackCheck = noOfSnacks[1] - reqSnackQty;
			}

			/*CONFIRM_SELECTION PHASE -  Calculating Price and Qty */

			reqSnackPrice = snackPrice[1] * reqSnackQty; // snack price * quantity of snack = reqSnackPrice

			EcsDrawBMP(img_CONFIRMSELECTION, pScrMgr);
			LcdClear();
			itoa((int)reqSnackQty, s, 10);
			LcdLine1(); LcdMsg("Qty Selected:  ");
			LcdMsg(s);
			// Convert double to char*
			char temp[sizeof(reqSnackPrice)];
			sprintf(temp, "%0.2f", reqSnackPrice);
			LcdLine2(); LcdMsg("Price: ");
			LcdMsg(temp);


			confirm = promptConfirmSelection(confirm); //need to use this for payment to enter payment phase
			if (confirm != 1) {
				EcsDrawBMP(img_SELECTIONCLEAR, pScrMgr); // clear selection
				reqSnackPrice = 0.0;
				reqSnackQty = 0;
				LcdClear();
				LcdLine1(); LcdMsg("   SELECTION   ");
				LcdLine2(); LcdMsg("    CLEARED    ");
				continue;    // will go back to start phase 
			} // skipped this if confirm == 1




			/* NEXT_TO PAYMENT_ADD SNACK PHASE */
			/* STORING THE NECESSARY DATA */
			if (selectionAttempt == 0) { // if user first selection attempt
				// Confirm is equal to 1
				EcsDrawBMP(img_NEXTSNACKPAYMENT, pScrMgr);
				LcdClear();
				LcdLine1(); LcdMsg("   SELECTION   ");
				LcdLine2(); LcdMsg("   CONFIRMED   ");
				deductNoOfSnacks(noOfSnacks, 1, reqSnackQty); // deduct snack quantity
				selectionOneQTY = reqSnackQty; // storing first final snack selection qty
				selectionOnePRICE = reqSnackPrice; // storing first final selection price
				totalSnackPrice += reqSnackPrice; // storing final price
				selectionAttempt = 1; // selectionAttempt = 1
				if (promptUserAddSnackPayment() == 1) {
					confirm = 0; // Add snack
					continue;
				}
				else if (promptUserAddSnackPayment() == 2) {
					// process payment
					// fallthrough to payment phase
				}
			}
			else if (selectionAttempt == 1) { // if user second seletion attempt
				// Confirm is equal to 1
				EcsDrawBMP(img_SELECTEDTWICE, pScrMgr);
				LcdClear();
				LcdLine1(); LcdMsg("   SELECTION   ");
				LcdLine2(); LcdMsg("   CONFIRMED   ");
				deductNoOfSnacks(noOfSnacks, 1, reqSnackQty); // deduct snack quantity
				selectionTwoQTY = reqSnackQty;
				selectionTwoPRICE = reqSnackPrice;
				totalSnackPrice += reqSnackPrice;
				selectionAttempt = 2;
				// fallthrough to payment phase
			}

			/******************************************************************************

											PAYMENT PHASE

			*******************************************************************************/

			if (confirm = 1) {
				EcsDrawBMP(img_PAYMENT, pScrMgr);
				LcdClear();
				LcdLine1(); LcdMsg("  TOTAL PRICE:  ");
				char temp[sizeof(totalSnackPrice)];
				sprintf(temp, "%0.2f", totalSnackPrice);
				LcdLine2(); LcdMsg("     ");
				LcdMsg(temp);
				if (promptUserToPay() == 1) {
					for (int i = 0; i < 6; i++) {
						if (i == 0) {
							EcsDrawBMP(img_PP1, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg(" PROCESSING .  ");
							Sleep(500);
						}
						else if (i == 1) {
							EcsDrawBMP(img_PP2, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg("PROCESSING ..  ");
							Sleep(500);
						}
						else if (i == 2) {
							EcsDrawBMP(img_PP3, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg("PROCESSING ...  ");
							Sleep(500);
						}
						else if (i == 3) {
							EcsDrawBMP(img_PP1, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg(" PROCESSING .  ");
							Sleep(500);
						}
						else if (i == 4) {
							EcsDrawBMP(img_PP2, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg("PROCESSING ..  ");
							Sleep(500);
						}
						else if (i == 5) {
							EcsDrawBMP(img_PP3, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg("PROCESSING ...  ");
							Sleep(500);
						}

					}
					//fallthrough
				}
			}

			EcsDrawBMP(img_PAYMENTSUCCESSFUL, pScrMgr);
			LcdClear();
			LcdLine1(); LcdMsg("    PAYMENT    ");
			LcdLine2(); LcdMsg("  SUCCESSFUL!   ");
			Sleep(1500);

			// rotate the motors
			EcsDrawBMP(img_SNACKDISPENSE, pScrMgr);
			LcdClear();
			LcdLine1(); LcdMsg("     SNACK     ");
			LcdLine2(); LcdMsg("   DISPENSING  ");
			Sleep(1500);
			Spin(CW, 300, 10); // 1.5 rotation

			// 
			EcsDrawBMP(img_ENJOY, pScrMgr);
			LcdClear();
			LcdLine1(); LcdMsg("     ENJOY    ");
			LcdLine2(); LcdMsg("  YOUR SNACK!  ");
			PlaySong((const char*)Hawaii);
			Sleep(1500);

			//reset the start screen
			start_end = 0;
			selectionAttempt = 0;
			totalSnackPrice = 0;
			break;
		case 2: // When user select Doritos $3
			if (noOfSnacks[2] <= 0) {
				EcsDrawBMP(img_SNACKNOTAVAIL, pScrMgr); // Display snack not available 
				itoa((int)noOfSnacks[2], s, 10);
				LcdClear();
				LcdLine1(); LcdMsg("    Doritos    ");
				LcdLine2(); LcdMsg("Qty: "); LcdMsg(s);
				Sleep(1000);
				//break; // this will break the switch case if noOfSnacks[1] < 1;
				if (selectionAttempt == 0) {
					start_end = 0;
				}
				else if (selectionAttempt == 1) {
					start_end = 1;
				}

				continue;
			}
			itoa((int)noOfSnacks[2], s, 10); // itoa ( int value, char * str, int base ), Convert integer to string (non-standard function)

			/* SELECT_QUANTITY PHASE */

			EcsDrawBMP(img_SELECTQUANTITY, pScrMgr);
			LcdClear();
			LcdLine1(); LcdMsg("    Doritos    ");
			LcdLine2(); LcdMsg("Qty: "); LcdMsg(s);
			Sleep(3000);

			reqSnackQty = promptNoOfSnacks(); // will return either 1/2/3

			snackCheck = noOfSnacks[2] - reqSnackQty; // Deduct the balance

			while ((snackCheck) < 0) { // Check if the user selection is valid 
				LcdClear();
				LcdLine1(); LcdMsg("Insuficient Qty");
				LcdLine2(); LcdMsg("Qty Left: ");
				LcdMsg(s);
				Sleep(2000);
				LcdClear();
				LcdLine1(); LcdMsg("     Please     ");
				LcdLine2(); LcdMsg("  Select Again  ");
				Sleep(2000);
				LcdClear();

				reqSnackQty = promptNoOfSnacks();
				snackCheck = noOfSnacks[2] - reqSnackQty; // Deduct the balance
				if (snackCheck > 0)
					break;

				//snackCheck = noOfSnacks[1] - reqSnackQty;
			}

			/*CONFIRM_SELECTION PHASE -  Calculating Price and Qty */

			reqSnackPrice = snackPrice[2] * reqSnackQty; // snack price * quantity of snack = reqSnackPrice

			EcsDrawBMP(img_CONFIRMSELECTION, pScrMgr);
			LcdClear();
			itoa((int)reqSnackQty, s, 10);
			LcdLine1(); LcdMsg("Qty Selected:  ");
			LcdMsg(s);
			// Convert double to char*
			temp[sizeof(reqSnackPrice)];
			sprintf(temp, "%0.2f", reqSnackPrice);
			LcdLine2(); LcdMsg("Price: ");
			LcdMsg(temp);


			confirm = promptConfirmSelection(confirm); //need to use this for payment to enter payment phase
			if (confirm != 1) {
				EcsDrawBMP(img_SELECTIONCLEAR, pScrMgr); // clear selection
				reqSnackPrice = 0.0;
				reqSnackQty = 0;
				LcdClear();
				LcdLine1(); LcdMsg("   SELECTION   ");
				LcdLine2(); LcdMsg("    CLEARED    ");
				continue;    // will go back to start phase 
			} // skipped this if confirm == 1




			/* NEXT_TO PAYMENT_ADD SNACK PHASE */
			/* STORING THE NECESSARY DATA */
			if (selectionAttempt == 0) { // if user first selection attempt
				// Confirm is equal to 1
				EcsDrawBMP(img_NEXTSNACKPAYMENT, pScrMgr);
				LcdClear();
				LcdLine1(); LcdMsg("   SELECTION   ");
				LcdLine2(); LcdMsg("   CONFIRMED   ");
				deductNoOfSnacks(noOfSnacks, 2, reqSnackQty); // deduct snack quantity
				selectionOneQTY = reqSnackQty; // storing first final snack selection qty
				selectionOnePRICE = reqSnackPrice; // storing first final selection price
				totalSnackPrice += reqSnackPrice; // storing final price
				selectionAttempt = 1; // selectionAttempt = 1
				if (promptUserAddSnackPayment() == 1) {
					confirm = 0; // Add snack
					continue;
				}
				else if (promptUserAddSnackPayment() == 2) {
					// process payment
					// fallthrough to payment phase
				}
			}
			else if (selectionAttempt == 1) { // if user second seletion attempt
				// Confirm is equal to 1
				EcsDrawBMP(img_SELECTEDTWICE, pScrMgr);
				LcdClear();
				LcdLine1(); LcdMsg("   SELECTION   ");
				LcdLine2(); LcdMsg("   CONFIRMED   ");
				deductNoOfSnacks(noOfSnacks, 2, reqSnackQty); // deduct snack quantity
				selectionTwoQTY = reqSnackQty;
				selectionTwoPRICE = reqSnackPrice;
				totalSnackPrice += reqSnackPrice;
				selectionAttempt = 2;
				// fallthrough to payment phase
			}

			/******************************************************************************

											PAYMENT PHASE

			*******************************************************************************/

			if (confirm = 1) {
				EcsDrawBMP(img_PAYMENT, pScrMgr);
				LcdClear();
				LcdLine1(); LcdMsg("  TOTAL PRICE:  ");
				char temp[sizeof(totalSnackPrice)];
				sprintf(temp, "%0.2f", totalSnackPrice);
				LcdLine2(); LcdMsg("     ");
				LcdMsg(temp);
				if (promptUserToPay() == 1) {
					for (int i = 0; i < 6; i++) {
						if (i == 0) {
							EcsDrawBMP(img_PP1, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg(" PROCESSING .  ");
							Sleep(500);
						}
						else if (i == 1) {
							EcsDrawBMP(img_PP2, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg("PROCESSING ..  ");
							Sleep(500);
						}
						else if (i == 2) {
							EcsDrawBMP(img_PP3, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg("PROCESSING ...  ");
							Sleep(500);
						}
						else if (i == 3) {
							EcsDrawBMP(img_PP1, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg(" PROCESSING .  ");
							Sleep(500);
						}
						else if (i == 4) {
							EcsDrawBMP(img_PP2, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg("PROCESSING ..  ");
							Sleep(500);
						}
						else if (i == 5) {
							EcsDrawBMP(img_PP3, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg("PROCESSING ...  ");
							Sleep(500);
						}

					}
					//fallthrough
				}
			}

			EcsDrawBMP(img_PAYMENTSUCCESSFUL, pScrMgr);
			LcdClear();
			LcdLine1(); LcdMsg("    PAYMENT    ");
			LcdLine2(); LcdMsg("  SUCCESSFUL!   ");
			Sleep(1500);

			// rotate the motors
			EcsDrawBMP(img_SNACKDISPENSE, pScrMgr);
			LcdClear();
			LcdLine1(); LcdMsg("     SNACK     ");
			LcdLine2(); LcdMsg("   DISPENSING  ");
			Sleep(1500);
			Spin(CW, 300, 10); // 1.5 rotation

			// 
			EcsDrawBMP(img_ENJOY, pScrMgr);
			LcdClear();
			LcdLine1(); LcdMsg("     ENJOY    ");
			LcdLine2(); LcdMsg("  YOUR SNACK!  ");
			PlaySong((const char*)Hawaii);
			Sleep(1500);

			//reset the start screen
			start_end = 0;
			selectionAttempt = 0;
			totalSnackPrice = 0;
			break;

		case 3: // When user select Lays $2.5
			if (noOfSnacks[3] <= 0) {
				EcsDrawBMP(img_SNACKNOTAVAIL, pScrMgr); // Display snack not available 
				itoa((int)noOfSnacks[3], s, 10);
				LcdClear();
				LcdLine1(); LcdMsg("      Lays    ");
				LcdLine2(); LcdMsg("Qty: "); LcdMsg(s);
				Sleep(1000);
				//break; // this will break the switch case if noOfSnacks[1] < 1;
				if (selectionAttempt == 0) {
					start_end = 0;
				}
				else if (selectionAttempt == 1) {
					start_end = 1;
				}

				continue;
			}
			itoa((int)noOfSnacks[3], s, 10); // itoa ( int value, char * str, int base ), Convert integer to string (non-standard function)

			/* SELECT_QUANTITY PHASE */

			EcsDrawBMP(img_SELECTQUANTITY, pScrMgr);
			LcdClear();
			LcdLine1(); LcdMsg("      Lays    ");
			LcdLine2(); LcdMsg("Qty: "); LcdMsg(s);
			Sleep(3000);

			reqSnackQty = promptNoOfSnacks(); // will return either 1/2/3

			snackCheck = noOfSnacks[3] - reqSnackQty; // Deduct the balance

			while ((snackCheck) < 0) { // Check if the user selection is valid 
				LcdClear();
				LcdLine1(); LcdMsg("Insuficient Qty");
				LcdLine2(); LcdMsg("Qty Left: ");
				LcdMsg(s);
				Sleep(2000);
				LcdClear();
				LcdLine1(); LcdMsg("     Please     ");
				LcdLine2(); LcdMsg("  Select Again  ");
				Sleep(2000);
				LcdClear();

				reqSnackQty = promptNoOfSnacks();
				snackCheck = noOfSnacks[3] - reqSnackQty; // Deduct the balance
				if (snackCheck > 0)
					break;

				//snackCheck = noOfSnacks[1] - reqSnackQty;
			}

			/*CONFIRM_SELECTION PHASE -  Calculating Price and Qty */

			reqSnackPrice = snackPrice[2] * reqSnackQty; // snack price * quantity of snack = reqSnackPrice

			EcsDrawBMP(img_CONFIRMSELECTION, pScrMgr);
			LcdClear();
			itoa((int)reqSnackQty, s, 10);
			LcdLine1(); LcdMsg("Qty Selected:  ");
			LcdMsg(s);
			// Convert double to char*
			temp[sizeof(reqSnackPrice)];
			sprintf(temp, "%0.2f", reqSnackPrice);
			LcdLine2(); LcdMsg("Price: ");
			LcdMsg(temp);


			confirm = promptConfirmSelection(confirm); //need to use this for payment to enter payment phase
			if (confirm != 1) {
				EcsDrawBMP(img_SELECTIONCLEAR, pScrMgr); // clear selection
				reqSnackPrice = 0.0;
				reqSnackQty = 0;
				LcdClear();
				LcdLine1(); LcdMsg("   SELECTION   ");
				LcdLine2(); LcdMsg("    CLEARED    ");
				continue;    // will go back to start phase 
			} // skipped this if confirm == 1




			/* NEXT_TO PAYMENT_ADD SNACK PHASE */
			/* STORING THE NECESSARY DATA */
			if (selectionAttempt == 0) { // if user first selection attempt
				// Confirm is equal to 1
				EcsDrawBMP(img_NEXTSNACKPAYMENT, pScrMgr);
				LcdClear();
				LcdLine1(); LcdMsg("   SELECTION   ");
				LcdLine2(); LcdMsg("   CONFIRMED   ");
				deductNoOfSnacks(noOfSnacks, 3, reqSnackQty); // deduct snack quantity
				selectionOneQTY = reqSnackQty; // storing first final snack selection qty
				selectionOnePRICE = reqSnackPrice; // storing first final selection price
				totalSnackPrice += reqSnackPrice; // storing final price
				selectionAttempt = 1; // selectionAttempt = 1
				if (promptUserAddSnackPayment() == 1) {
					confirm = 0; // Add snack
					continue;
				}
				else if (promptUserAddSnackPayment() == 2) {
					// process payment
					// fallthrough to payment phase
				}
			}
			else if (selectionAttempt == 1) { // if user second seletion attempt
				// Confirm is equal to 1
				EcsDrawBMP(img_SELECTEDTWICE, pScrMgr);
				LcdClear();
				LcdLine1(); LcdMsg("   SELECTION   ");
				LcdLine2(); LcdMsg("   CONFIRMED   ");
				deductNoOfSnacks(noOfSnacks, 3, reqSnackQty); // deduct snack quantity
				selectionTwoQTY = reqSnackQty;
				selectionTwoPRICE = reqSnackPrice;
				totalSnackPrice += reqSnackPrice;
				selectionAttempt = 2;
				// fallthrough to payment phase
			}

			/******************************************************************************

											PAYMENT PHASE

			*******************************************************************************/

			if (confirm = 1) {
				EcsDrawBMP(img_PAYMENT, pScrMgr);
				LcdClear();
				LcdLine1(); LcdMsg("  TOTAL PRICE:  ");
				char temp[sizeof(totalSnackPrice)];
				sprintf(temp, "%0.2f", totalSnackPrice);
				LcdLine2(); LcdMsg("     ");
				LcdMsg(temp);
				if (promptUserToPay() == 1) {
					for (int i = 0; i < 6; i++) {
						if (i == 0) {
							EcsDrawBMP(img_PP1, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg(" PROCESSING .  ");
							Sleep(500);
						}
						else if (i == 1) {
							EcsDrawBMP(img_PP2, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg("PROCESSING ..  ");
							Sleep(500);
						}
						else if (i == 2) {
							EcsDrawBMP(img_PP3, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg("PROCESSING ...  ");
							Sleep(500);
						}
						else if (i == 3) {
							EcsDrawBMP(img_PP1, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg(" PROCESSING .  ");
							Sleep(500);
						}
						else if (i == 4) {
							EcsDrawBMP(img_PP2, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg("PROCESSING ..  ");
							Sleep(500);
						}
						else if (i == 5) {
							EcsDrawBMP(img_PP3, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg("PROCESSING ...  ");
							Sleep(500);
						}

					}
					//fallthrough
				}
			}

			EcsDrawBMP(img_PAYMENTSUCCESSFUL, pScrMgr);
			LcdClear();
			LcdLine1(); LcdMsg("    PAYMENT    ");
			LcdLine2(); LcdMsg("  SUCCESSFUL!   ");
			Sleep(1500);

			// rotate the motors
			EcsDrawBMP(img_SNACKDISPENSE, pScrMgr);
			LcdClear();
			LcdLine1(); LcdMsg("     SNACK     ");
			LcdLine2(); LcdMsg("   DISPENSING  ");
			Sleep(1500);
			Spin(CW, 300, 10); // 1.5 rotation

			// 
			EcsDrawBMP(img_ENJOY, pScrMgr);
			LcdClear();
			LcdLine1(); LcdMsg("     ENJOY    ");
			LcdLine2(); LcdMsg("  YOUR SNACK!  ");
			PlaySong((const char*)Hawaii);
			Sleep(1500);

			//reset the start screen
			start_end = 0;
			selectionAttempt = 0;
			totalSnackPrice = 0;
			break;
		case 4: // When user select Mentos $.5
			if (noOfSnacks[4] <= 0) {
				EcsDrawBMP(img_SNACKNOTAVAIL, pScrMgr); // Display snack not available 
				itoa((int)noOfSnacks[4], s, 10);
				LcdClear();
				LcdLine1(); LcdMsg("     Mentos    ");
				LcdLine2(); LcdMsg("Qty: "); LcdMsg(s);
				Sleep(1000);
				//break; // this will break the switch case if noOfSnacks[1] < 1;
				if (selectionAttempt == 0) {
					start_end = 0;
				}
				else if (selectionAttempt == 1) {
					start_end = 1;
				}

				continue;
			}
			itoa((int)noOfSnacks[4], s, 10); // itoa ( int value, char * str, int base ), Convert integer to string (non-standard function)

			/* SELECT_QUANTITY PHASE */

			EcsDrawBMP(img_SELECTQUANTITY, pScrMgr);
			LcdClear();
			LcdLine1(); LcdMsg("     Mentos    ");
			LcdLine2(); LcdMsg("Qty: "); LcdMsg(s);
			Sleep(3000);

			reqSnackQty = promptNoOfSnacks(); // will return either 1/2/3

			snackCheck = noOfSnacks[4] - reqSnackQty; // Deduct the balance

			while ((snackCheck) < 0) { // Check if the user selection is valid 
				LcdClear();
				LcdLine1(); LcdMsg("Insuficient Qty");
				LcdLine2(); LcdMsg("Qty Left: ");
				LcdMsg(s);
				Sleep(2000);
				LcdClear();
				LcdLine1(); LcdMsg("     Please     ");
				LcdLine2(); LcdMsg("  Select Again  ");
				Sleep(2000);
				LcdClear();

				reqSnackQty = promptNoOfSnacks();
				snackCheck = noOfSnacks[4] - reqSnackQty; // Deduct the balance
				if (snackCheck > 0)
					break;

				//snackCheck = noOfSnacks[1] - reqSnackQty;
			}

			/*CONFIRM_SELECTION PHASE -  Calculating Price and Qty */

			reqSnackPrice = snackPrice[4] * reqSnackQty; // snack price * quantity of snack = reqSnackPrice

			EcsDrawBMP(img_CONFIRMSELECTION, pScrMgr);
			LcdClear();
			itoa((int)reqSnackQty, s, 10);
			LcdLine1(); LcdMsg("Qty Selected:  ");
			LcdMsg(s);
			// Convert double to char*
			temp[sizeof(reqSnackPrice)];
			sprintf(temp, "%0.2f", reqSnackPrice);
			LcdLine2(); LcdMsg("Price: ");
			LcdMsg(temp);


			confirm = promptConfirmSelection(confirm); //need to use this for payment to enter payment phase
			if (confirm != 1) {
				EcsDrawBMP(img_SELECTIONCLEAR, pScrMgr); // clear selection
				reqSnackPrice = 0.0;
				reqSnackQty = 0;
				LcdClear();
				LcdLine1(); LcdMsg("   SELECTION   ");
				LcdLine2(); LcdMsg("    CLEARED    ");
				continue;    // will go back to start phase 
			} // skipped this if confirm == 1




			/* NEXT_TO PAYMENT_ADD SNACK PHASE */
			/* STORING THE NECESSARY DATA */
			if (selectionAttempt == 0) { // if user first selection attempt
				// Confirm is equal to 1
				EcsDrawBMP(img_NEXTSNACKPAYMENT, pScrMgr);
				LcdClear();
				LcdLine1(); LcdMsg("   SELECTION   ");
				LcdLine2(); LcdMsg("   CONFIRMED   ");
				deductNoOfSnacks(noOfSnacks, 4, reqSnackQty); // deduct snack quantity
				selectionOneQTY = reqSnackQty; // storing first final snack selection qty
				selectionOnePRICE = reqSnackPrice; // storing first final selection price
				totalSnackPrice += reqSnackPrice; // storing final price
				selectionAttempt = 1; // selectionAttempt = 1
				if (promptUserAddSnackPayment() == 1) {
					confirm = 0; // Add snack
					continue;
				}
				else if (promptUserAddSnackPayment() == 2) {
					// process payment
					// fallthrough to payment phase
				}
			}
			else if (selectionAttempt == 1) { // if user second seletion attempt
				// Confirm is equal to 1
				EcsDrawBMP(img_SELECTEDTWICE, pScrMgr);
				LcdClear();
				LcdLine1(); LcdMsg("   SELECTION   ");
				LcdLine2(); LcdMsg("   CONFIRMED   ");
				deductNoOfSnacks(noOfSnacks, 4, reqSnackQty); // deduct snack quantity
				selectionTwoQTY = reqSnackQty;
				selectionTwoPRICE = reqSnackPrice;
				totalSnackPrice += reqSnackPrice;
				selectionAttempt = 2;
				// fallthrough to payment phase
			}

			/******************************************************************************

											PAYMENT PHASE

			*******************************************************************************/

			if (confirm = 1) {
				EcsDrawBMP(img_PAYMENT, pScrMgr);
				LcdClear();
				LcdLine1(); LcdMsg("  TOTAL PRICE:  ");
				char temp[sizeof(totalSnackPrice)];
				sprintf(temp, "%0.2f", totalSnackPrice);
				LcdLine2(); LcdMsg("     ");
				LcdMsg(temp);
				if (promptUserToPay() == 1) {
					for (int i = 0; i < 6; i++) {
						if (i == 0) {
							EcsDrawBMP(img_PP1, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg(" PROCESSING .  ");
							Sleep(500);
						}
						else if (i == 1) {
							EcsDrawBMP(img_PP2, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg("PROCESSING ..  ");
							Sleep(500);
						}
						else if (i == 2) {
							EcsDrawBMP(img_PP3, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg("PROCESSING ...  ");
							Sleep(500);
						}
						else if (i == 3) {
							EcsDrawBMP(img_PP1, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg(" PROCESSING .  ");
							Sleep(500);
						}
						else if (i == 4) {
							EcsDrawBMP(img_PP2, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg("PROCESSING ..  ");
							Sleep(500);
						}
						else if (i == 5) {
							EcsDrawBMP(img_PP3, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg("PROCESSING ...  ");
							Sleep(500);
						}

					}
					//fallthrough
				}
			}

			EcsDrawBMP(img_PAYMENTSUCCESSFUL, pScrMgr);
			LcdClear();
			LcdLine1(); LcdMsg("    PAYMENT    ");
			LcdLine2(); LcdMsg("  SUCCESSFUL!   ");
			Sleep(1500);

			// rotate the motors
			EcsDrawBMP(img_SNACKDISPENSE, pScrMgr);
			LcdClear();
			LcdLine1(); LcdMsg("     SNACK     ");
			LcdLine2(); LcdMsg("   DISPENSING  ");
			Sleep(1500);
			Spin(CW, 300, 10); // 1.5 rotation

			// 
			EcsDrawBMP(img_ENJOY, pScrMgr);
			LcdClear();
			LcdLine1(); LcdMsg("     ENJOY    ");
			LcdLine2(); LcdMsg("  YOUR SNACK!  ");
			PlaySong((const char*)Hawaii);
			Sleep(1500);

			//reset the start screen
			start_end = 0;
			selectionAttempt = 0;
			totalSnackPrice = 0;
			break;
		case 5: // When user select Haribo $1.5
			if (noOfSnacks[5] <= 0) {
				EcsDrawBMP(img_SNACKNOTAVAIL, pScrMgr); // Display snack not available 
				itoa((int)noOfSnacks[5], s, 10);
				LcdClear();
				LcdLine1(); LcdMsg("     Haribo    ");
				LcdLine2(); LcdMsg("Qty: "); LcdMsg(s);
				Sleep(1000);
				//break; // this will break the switch case if noOfSnacks[1] < 1;
				if (selectionAttempt == 0) {
					start_end = 0;
				}
				else if (selectionAttempt == 1) {
					start_end = 1;
				}

				continue;
			}
			itoa((int)noOfSnacks[5], s, 10); // itoa ( int value, char * str, int base ), Convert integer to string (non-standard function)

			/* SELECT_QUANTITY PHASE */

			EcsDrawBMP(img_SELECTQUANTITY, pScrMgr);
			LcdClear();
			LcdLine1(); LcdMsg("     Haribo    ");
			LcdLine2(); LcdMsg("Qty: "); LcdMsg(s);
			Sleep(3000);

			reqSnackQty = promptNoOfSnacks(); // will return either 1/2/3

			snackCheck = noOfSnacks[5] - reqSnackQty; // Deduct the balance

			while ((snackCheck) < 0) { // Check if the user selection is valid 
				LcdClear();
				LcdLine1(); LcdMsg("Insuficient Qty");
				LcdLine2(); LcdMsg("Qty Left: ");
				LcdMsg(s);
				Sleep(2000);
				LcdClear();
				LcdLine1(); LcdMsg("     Please     ");
				LcdLine2(); LcdMsg("  Select Again  ");
				Sleep(2000);
				LcdClear();

				reqSnackQty = promptNoOfSnacks();
				snackCheck = noOfSnacks[5] - reqSnackQty; // Deduct the balance
				if (snackCheck > 0)
					break;

				//snackCheck = noOfSnacks[1] - reqSnackQty;
			}

			/*CONFIRM_SELECTION PHASE -  Calculating Price and Qty */

			reqSnackPrice = snackPrice[5] * reqSnackQty; // snack price * quantity of snack = reqSnackPrice

			EcsDrawBMP(img_CONFIRMSELECTION, pScrMgr);
			LcdClear();
			itoa((int)reqSnackQty, s, 10);
			LcdLine1(); LcdMsg("Qty Selected:  ");
			LcdMsg(s);
			// Convert double to char*
			temp[sizeof(reqSnackPrice)];
			sprintf(temp, "%0.2f", reqSnackPrice);
			LcdLine2(); LcdMsg("Price: ");
			LcdMsg(temp);


			confirm = promptConfirmSelection(confirm); //need to use this for payment to enter payment phase
			if (confirm != 1) {
				EcsDrawBMP(img_SELECTIONCLEAR, pScrMgr); // clear selection
				reqSnackPrice = 0.0;
				reqSnackQty = 0;
				LcdClear();
				LcdLine1(); LcdMsg("   SELECTION   ");
				LcdLine2(); LcdMsg("    CLEARED    ");
				continue;    // will go back to start phase 
			} // skipped this if confirm == 1




			/* NEXT_TO PAYMENT_ADD SNACK PHASE */
			/* STORING THE NECESSARY DATA */
			if (selectionAttempt == 0) { // if user first selection attempt
				// Confirm is equal to 1
				EcsDrawBMP(img_NEXTSNACKPAYMENT, pScrMgr);
				LcdClear();
				LcdLine1(); LcdMsg("   SELECTION   ");
				LcdLine2(); LcdMsg("   CONFIRMED   ");
				deductNoOfSnacks(noOfSnacks, 5, reqSnackQty); // deduct snack quantity
				selectionOneQTY = reqSnackQty; // storing first final snack selection qty
				selectionOnePRICE = reqSnackPrice; // storing first final selection price
				totalSnackPrice += reqSnackPrice; // storing final price
				selectionAttempt = 1; // selectionAttempt = 1
				if (promptUserAddSnackPayment() == 1) {
					confirm = 0; // Add snack
					continue;
				}
				else if (promptUserAddSnackPayment() == 2) {
					// process payment
					// fallthrough to payment phase
				}
			}
			else if (selectionAttempt == 1) { // if user second seletion attempt
				// Confirm is equal to 1
				EcsDrawBMP(img_SELECTEDTWICE, pScrMgr);
				LcdClear();
				LcdLine1(); LcdMsg("   SELECTION   ");
				LcdLine2(); LcdMsg("   CONFIRMED   ");
				deductNoOfSnacks(noOfSnacks, 5, reqSnackQty); // deduct snack quantity
				selectionTwoQTY = reqSnackQty;
				selectionTwoPRICE = reqSnackPrice;
				totalSnackPrice += reqSnackPrice;
				selectionAttempt = 2;
				// fallthrough to payment phase
			}

			/******************************************************************************

											PAYMENT PHASE

			*******************************************************************************/

			if (confirm = 1) {
				EcsDrawBMP(img_PAYMENT, pScrMgr);
				LcdClear();
				LcdLine1(); LcdMsg("  TOTAL PRICE:  ");
				char temp[sizeof(totalSnackPrice)];
				sprintf(temp, "%0.2f", totalSnackPrice);
				LcdLine2(); LcdMsg("     ");
				LcdMsg(temp);
				if (promptUserToPay() == 1) {
					for (int i = 0; i < 6; i++) {
						if (i == 0) {
							EcsDrawBMP(img_PP1, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg(" PROCESSING .  ");
							Sleep(500);
						}
						else if (i == 1) {
							EcsDrawBMP(img_PP2, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg("PROCESSING ..  ");
							Sleep(500);
						}
						else if (i == 2) {
							EcsDrawBMP(img_PP3, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg("PROCESSING ...  ");
							Sleep(500);
						}
						else if (i == 3) {
							EcsDrawBMP(img_PP1, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg(" PROCESSING .  ");
							Sleep(500);
						}
						else if (i == 4) {
							EcsDrawBMP(img_PP2, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg("PROCESSING ..  ");
							Sleep(500);
						}
						else if (i == 5) {
							EcsDrawBMP(img_PP3, pScrMgr);
							LcdClear();
							LcdLine1(); LcdMsg("    PAYMENT    ");
							LcdLine2(); LcdMsg("PROCESSING ...  ");
							Sleep(500);
						}

					}
					//fallthrough
				}
			}

			EcsDrawBMP(img_PAYMENTSUCCESSFUL, pScrMgr);
			LcdClear();
			LcdLine1(); LcdMsg("    PAYMENT    ");
			LcdLine2(); LcdMsg("  SUCCESSFUL!   ");
			Sleep(1500);

			// rotate the motors
			EcsDrawBMP(img_SNACKDISPENSE, pScrMgr);
			LcdClear();
			LcdLine1(); LcdMsg("     SNACK     ");
			LcdLine2(); LcdMsg("   DISPENSING  ");
			Sleep(1500);
			Spin(CW, 300, 10); // 1.5 rotation

			// 
			EcsDrawBMP(img_ENJOY, pScrMgr);
			LcdClear();
			LcdLine1(); LcdMsg("     ENJOY    ");
			LcdLine2(); LcdMsg("  YOUR SNACK!  ");
			PlaySong((const char*)Hawaii);
			Sleep(1500);

			//reset the start screen
			start_end = 0;
			selectionAttempt = 0;
			totalSnackPrice = 0;
			break;
		

		default:
			//printf("Press select 1 to 6");
			LcdLine1(); LcdMsg("    Wrong Key    ");
			LcdLine2(); LcdMsg("		1 to 6      "); Sleep(1000);
			break;
		}
		// LcdLine1();LcdMsg(" One more time  ");
		// LcdLine2();LcdMsg("Press a Key  ...");

	}
}


int promptNoOfSnacks() { // Prompt user to decide the number of snack
	int snack = 0;
	while (snack == 0) {
		Sleep(10);
		switch (KeyBeep()) {
		case 1:
			snack = 1;
			break;
		case 2:
			snack = 2;
			break;
		case 3:
			snack = 3;
			break;
		default:
			LcdClear();
			LcdLine1(); LcdMsg("     WRONG     ");
			LcdLine2(); LcdMsg("      KEY      "); Sleep(2000);
			LcdClear();
			LcdLine1(); LcdMsg("     SELECT     ");
			LcdLine2(); LcdMsg("  1 OR 2 OR 3  "); Sleep(2000);
			break;

		}
	}
	return snack;
}

bool promptConfirmSelection(bool confirm) {
	Sleep(10);
	switch (KeyBeep()) {
	case 0:
		confirm = 0; // not confirm selection
		break;
	case 1:
		confirm = 1; // confirm seletion
		break;
	default:
		LcdClear();
		LcdLine1(); LcdMsg("     WRONG     ");
		LcdLine2(); LcdMsg("      KEY      "); Sleep(2000);
		LcdClear();
		LcdLine1(); LcdMsg("     SELECT     ");
		LcdLine2(); LcdMsg("     1 OR 0     "); Sleep(2000);
		break;

	}

	return confirm;
}

int promptUserAddSnackPayment() {
	Sleep(10);
	switch (KeyBeep()) {
	case 1:
		return 1; // To add snacks
		break;
	case 2:
		return 2; // To Payment
		break;
	default:
		LcdClear();
		LcdLine1(); LcdMsg("     WRONG     ");
		LcdLine2(); LcdMsg("      KEY      "); Sleep(2000);
		LcdClear();
		LcdLine1(); LcdMsg("     SELECT     ");
		LcdLine2(); LcdMsg("     1 OR 2     "); Sleep(2000);
		break;
	}
	return 0;
}

int promptUserToPay() {
	Sleep(10);
	switch (KeyBeep()) {
	case 1:
		return 1;
		break;
	default:
		LcdClear();
		LcdLine1(); LcdMsg("     WRONG     ");
		LcdLine2(); LcdMsg("      KEY      "); Sleep(2000);
		break;
	}
	return 0;
}

void paymentProcessing() {
	// Play song

}

void deductNoOfSnacks(int number[], int snackID, int no) {
	*(number + snackID) -= no;
}



Ubyte KeyBeep(void)
{
	Ubyte c;
	c = KeyPressed();
	LedDisp(c);
	BEEP();
	return c;
}

unsigned int __stdcall sm_thread(void* pArguments)
{
	while (1)
	{
		Spin(CW, 100, 10); // turn motor clockwise, 100 steps
		Spin(CCW, 100, 10); // turn motor counterclockwise, 100 steps
	}
	return(0);
}

unsigned int __stdcall music_thread(void* pArguments)
{

	while (TRUE)
	{
		PlaySong((const char*)MinuetG);
		Sleep(500);
		PlaySong((const char*)Hawaii);
		Sleep(500);
		PlaySong((const char*)TurkishRondo);
		Sleep(500);
	}
	return(0);
}
