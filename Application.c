/* 
 * file:   Paymenet Application
 * Author: Abdullah Hashish
 * Created on October 10, 2022
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"Application.h"

ST_cardData_t Card_1;
ST_transaction_t trans_obj;
ST_terminalData_t Terminal_1;

void main (void)
{   
    
    //for (uint32_t i=0;i<3;i++)Add_one_element();
    
    startapp();
    

}

void startapp (void)
{
    uint32_t flag=0,attempt=1;
    EN_cardError_t ret_card;
    EN_terminalError_t ret_terminal;
    EN_serverError_t ret_server;
    EN_transState_t ret_state;
    printf("\n");
    puts("############(Application START)############");
    while(flag==0)
    {
        /*-----------------------------------------------------------Start of Card module------------------------------------*/
        fflush(stdin);
        ret_card=getCardPAN(&Card_1);
        while (ret_card==WRONG_PAN && attempt<3)
        {
            printf("wrong PAN, please try again\n");
            ret_card=getCardPAN(&Card_1);
            attempt++;
            if (attempt==3)
            {
            puts("sorry you have exceeded the limit of attempts!\nTRY AGAIN LATER");
            flag=1;
            break;
            }
        }
        if (flag==1)break;

        ret_card=getCardHolderName(&Card_1);
        while (ret_card==WRONG_NAME)
        {
            printf("wrong Name format, please try again\n");
            ret_card=getCardHolderName(&Card_1);
        }

        ret_card=getCardExpiryDate(&Card_1);
        while (ret_card==WRONG_EXP_DATE)
        {
            printf("wrong date format, please try again\n");
            ret_card=getCardExpiryDate(&Card_1);
        }
        /*-------------------------------------------End of Card module-----------------------------------------------*/

        /*--------------------------------------------start of termina module---------------------------------------*/
        fflush(stdin);
        ret_terminal=getTransactionDate(&Terminal_1);
        if(ret_terminal>0) puts("wrong date");
        ret_terminal=isCardExpired(Card_1,Terminal_1);
        if(ret_terminal==EXPIRED_CARD)
        {
            puts("Expired Card please Check With one of our Clients ASAB");
            flag=1;
        }
        if (flag==1)break;
        ret_terminal=setMaxAmount(&Terminal_1);
        if(ret_terminal==INVALID_MAX_AMOUNT) break;
        ret_terminal=getTransactionAmount(&Terminal_1);
        while(ret_terminal==INVALID_AMOUNT)
        {
            printf("Invalid Amount\n");
            ret_terminal=getTransactionAmount(&Terminal_1);
        }

        ret_terminal=isBelowMaxAmount(&Terminal_1);
        if(ret_terminal==EXCEED_MAX_AMOUNT)
        {
            puts("Exceed available amount, please try again");
            flag=1;
        }
        if (flag==1)break;
        /*---------------------------------------End of terminal module-----------------------------------*/

        /*--------------------------------------start of Server module------------------------------------*/
        ret_server=FetchServerData(&trans_obj,&Card_1,&Terminal_1);
        if(ret_server==INTERNAL_SERVER_ERROR)
        {
            puts("INTERNAL_SERVER_ERROR,try again later");
            flag=1;
        }
        if(flag==1) break;

        ret_state=recieveTransactionData(&trans_obj);
        switch(ret_state)
        {
            case INTERNAL_SERVER_ERROR:
            puts("INTERNAL_SERVER_ERROR, try again later");
            break;

            case FRAUD_CARD:
            puts("FRAUD_CARD");
            break;

            
            case DECLINED_INSUFFECIENT_FUND:
            puts("there is no available balance, check current Balance");
            break;

            case DECLINED_STOLEN_CARD:
            puts("ACCOUNT IS SUSPENDED, please Check with our Clients");
            break;

            case APPROVED:
            puts("Transaction is Done Successefully\nthanks:)");
            break;
            
        }  
         ret_server=saveTransaction(&trans_obj);
            if (ret_server==SAVING_FAILED)
            {
                puts("SAVING_FAILD");
            }        
        /*-----------------------------------------END OF SREVER MODULE-------------------------------------*/
       puts("############(Application END)############\n");
    }
    
       
}
