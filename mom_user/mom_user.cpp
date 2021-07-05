//////////////////////////////////////////////////////////////////////////////
//
//  mom_user.cpp
//
//  Description:
//      Contains Unigraphics entry points for the application.
//
//////////////////////////////////////////////////////////////////////////////

// #define _CRT_SECURE_NO_DEPRECATE 1

//  Include files
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <uf.h>
#include <uf_exit.h>
#include <uf_ui.h>
#include <uf_mom.h>

#include "mom_user.h"

//----------------------------------------------------------------------------

/*
 * Защита через номер тома:
 */
int sub_10002920() ;
/*
 * Exported Tcl procedures:
 */
static int sub_10001080
(
  void *client_data,    /* This is the mom object */
  void *interp,         /* Pointer to the TCL interpreter */
  int  argc,            /* Count of the numnber of arguments */
  char **argv           /* Array of arguments where the
                           argv[0] - The TCL extension */
);

//----------------------------------------------------------------------------
//  Activation Methods
//----------------------------------------------------------------------------

//  Explicit Activation
//      This entry point is used to activate the application explicitly, as in
//      "File->Execute UG/Open->User Function..."
//----- (10001010) --------------------------------------------------------
extern "C" DllExport void entry_rtv_mom_cp_part_attr( char *parm, int *returnCode, int rlen )
{
  int errorCode ;
  UF_MOM_id_t mom ;
  void *interp = NULL ;

  if ( !UF_is_initialized() )
  {
    errorCode = UF_initialize();
    *returnCode = errorCode;
    if ( errorCode ) return ;
  }

  /* Get the TCL interpreter id from the ufusr param */
  UF_MOM_ask_interp_from_param ( parm, &interp ) ;
  /* Get the MOM id from the ufusr param */
  UF_MOM_ask_mom (parm, &mom) ;
  /*Создаем функции в интепретаторе*/
  UF_MOM_extend_xlator ( mom , "EXTN_rtv_mom_cp_part_attr", sub_10001080 ) ;

  *returnCode = 0;
  if ( errorCode == 1 )  errorCode = UF_terminate();
  *returnCode = errorCode;
}


//----------------------------------------------------------------------------
//  Utilities
//----------------------------------------------------------------------------

// Unload Handler
//     This function specifies when to unload your application from Unigraphics.
//     If your application registers a callback (from a MenuScript item or a
//     User Defined Object for example), this function MUST return
//     "UF_UNLOAD_UG_TERMINATE".
extern "C" int ufusr_ask_unload( void )
{
     /* unload immediately after application exits*/
     //return ( UF_UNLOAD_IMMEDIATELY );

     /*via the unload selection dialog... */
     //return ( UF_UNLOAD_SEL_DIALOG );
     /*when UG terminates...              */
     return ( UF_UNLOAD_UG_TERMINATE );
}


/*-----------------------------------------------------------------------
 * Function Name: sub_10001080
 *
 *
 -----------------------------------------------------------------------*/
static int sub_10001080
(
  void *client_data,    /* This is the mom object */
  void *interp,         /* Pointer to the TCL interpreter */
  int  argc,            /* Count of the numnber of arguments */
  char **argv           /* Array of arguments where the
                           argv[0] - The TCL extension */
)
{
  UF_MOM_id_t mom_id = ( UF_MOM_id_t ) client_data ;

  int     ii ;
  char    Dest[256] ;
  double  angle1 , angle2 ;
  double  x , y , z , i , j , k ;
  const char *kin_retract_distance ;
  double  retract_distance ;
  double  dx , dy , dz ;
  
  int  dword_10003334 = 0 ;

  if ( sub_10002920() == 0 )
  {
    const int Size = 1024 ;
    char  str[Size]="НА ЭТОМ РАБОЧЕМ МЕСТЕ ПОСТПРОЦЕССОР НЕ МОЖЕТ БЫТЬ ЗАПУЩЕН\n \
Москва, CSoft (ЗАО \"СиСофт\"),\nТелефон: +7 (495) 913-22-22, Факс: +7 (495) 913-22-21\n \
e-mail: jura@csoft.ru, g00@inbox.ru\nHTTP: http://www.csoft.ru\n\n(c) 2007.\n\n\nПоследняя редакция 2009\0";

    UF_UI_display_nonmodal_msg("Внимание!", str, UF_UI_MSG_POS_CURSOR);
    UF_terminate();
    return ( 0 ) ;
  }

    sprintf(Dest, "Number of args %ld", argc);
    if ( argc > 0 )
    {
      for (ii=0;ii<argc;ii++) sprintf(Dest, "Arg 0 %s", argv[ii]);
    }

    if (0==strcmp(argv[1],"PLANE"))
    {
      UF_MOM_ask_assoc_double_array(mom_id, "out_angle_pos", "0", &angle1);
      UF_MOM_ask_assoc_double_array(mom_id, "out_angle_pos", "1", &angle2);
      sprintf(Dest, "MOM_output_literal \"PLANE SPATIAL SPA+0 SPB%.3lf SPC%.3lf STAY TABLE ROT \"", angle1, angle2);
      UF_MOM_execute_command(mom_id, Dest);   
    }

    if (0==strcmp(argv[1],"PLANE_moving"))
    {
      UF_MOM_ask_assoc_double_array(mom_id, "mcs_goto", "0", &x);
      UF_MOM_ask_assoc_double_array(mom_id, "mcs_goto", "1", &y);
      UF_MOM_ask_assoc_double_array(mom_id, "mcs_goto", "2", &z);
      sprintf(Dest, "MOM_output_literal \"M128 \"");
      UF_MOM_execute_command(mom_id, Dest);
      sprintf(Dest, "MOM_output_literal \"L X%.3lf Y%.3lf F MAX\"", x, y);
      UF_MOM_execute_command(mom_id, Dest);
      sprintf(Dest, "MOM_output_literal \"L Z%.3lf F MAX\"", z);
      UF_MOM_execute_command(mom_id, Dest);
      sprintf(Dest, "MOM_output_literal \"M129 \"");
      UF_MOM_execute_command(mom_id, Dest);
    } 

    if (0==strcmp(argv[1],"INPUT_RETRACT"))
    {
      int  ia4[2]={0,0},
           ip6[2] = {1,1},
           resp;
      double ra5[2] = { 100, 100 } ;
      char   menu[2][16]; //={":Отвод=",":Врезание="};
      strcpy(&menu[0][0], ":Отвод (мм)=");
      strcpy(&menu[1][0], ":Врезание (мм)=");
      //ra5[0] = *n0;
      //ra5[1] = *n1;

      UF_UI_close_listing_window();
      UF_UI_lock_ug_access(UF_UI_FROM_CUSTOM);
      resp = uc1608(":Введите значения отвода и врезания инструмента:", menu, 2, ia4, ra5, ip6);
        //*n0 = ra5[0];
        //*n1 = ra5[1];
      UF_MOM_set_double(mom_id, "kin_retract_distance", ra5[0]);
      UF_MOM_set_double(mom_id, "kin_reengage_distance", ra5[1]);
      UF_UI_unlock_ug_access(UF_UI_FROM_CUSTOM);
    }

    if (0==strcmp(argv[1],"INPUT_RETRACT_DIST"))
    {
      uc1601(":Ввод дистанций отвода и врезания:", 0);
      UF_MOM_ask_assoc_double_array(mom_id, "prev_mcs_goto", "0", &x);
      UF_MOM_ask_assoc_double_array(mom_id, "prev_mcs_goto", "1", &y);
      UF_MOM_ask_assoc_double_array(mom_id, "prev_mcs_goto", "2", &z);
      UF_MOM_ask_assoc_double_array(mom_id, "prev_tool_axis", "0", &i);
      UF_MOM_ask_assoc_double_array(mom_id, "prev_tool_axis", "1", &j);
      UF_MOM_ask_assoc_double_array(mom_id, "prev_tool_axis", "2", &k);
      UF_MOM_ask_string(mom_id, "kin_retract_distance", &kin_retract_distance);
      retract_distance=atof(kin_retract_distance);
      //UF_free(&kin_retract_distance);

      dx=i*retract_distance + x ;
      dy=j*retract_distance + y ;
      dz=k*retract_distance + z ;

      UF_MOM_set_assoc_double_array(mom_id, "pos", "0", dx);
      UF_MOM_set_assoc_double_array(mom_id, "pos", "1", dy);
      UF_MOM_set_assoc_double_array(mom_id, "pos", "2", dz);
    }
  
    if ( !strcmp(argv[1], "M128") )
    {
      UF_MOM_ask_assoc_double_array(mom_id, "mcs_goto", "0", &x);
      UF_MOM_ask_assoc_double_array(mom_id, "mcs_goto", "1", &y);
      UF_MOM_ask_assoc_double_array(mom_id, "mcs_goto", "2", &z);
      UF_MOM_set_assoc_double_array(mom_id, "pos", "0", x);
      UF_MOM_set_assoc_double_array(mom_id, "pos", "1", y);
      UF_MOM_set_assoc_double_array(mom_id, "pos", "2", z);
    }

    if ( !strcmp(argv[1], "UNLOAD") ) dword_10003334 = 0;
    UF_MOM_set_string(mom_id, "var_unknow", Dest);
    UF_terminate(); 
  
  return ( 0 ) ;
}

//----- (10002920) --------------------------------------------------------
int sub_10002920()
{
  int i ;
  UF_system_info_t               zSystemInfo;
  UF_system_info_t               Info[9];
 /*
   "pc61-39" "GracyevaME"  "NX 6.0.0.24" 
   "pc61-36" "kmo" 
   "pc61-43 "user"    
   "pc61-30" "KnyazevaLG"  "NX 4.0.0.25" 
   "c26-1"   "KutaevaGV"   
   "pc61-15" "KalinychevVI"  "NX 4.0.3.3"  
   "chigishev-nb"    "Jura"    
   "batarev-nb"  "Batarev" "NX 4.0.4.2"
 */
  
  Info[0].user_name = "pc61-39";
  Info[0].program_name = "GracyevaME";
  Info[0].node_name = "NX 6.0.0.24";

  Info[1].user_name = "pc61-36";
  Info[1].program_name = "kmo";
  Info[1].node_name = NULL;
  
  // и так далее
   
  UF_ask_system_info(&zSystemInfo);
  
  i = 0;
  while ( strcmp(zSystemInfo.node_name, Info.node_name[i] ) )
  {
    ++i;
    if ( i >= 9 ) { UF_free_system_info(&zSystemInfo); return 0; }
  }
  UF_free_system_info(&zSystemInfo);
  return 1;
}