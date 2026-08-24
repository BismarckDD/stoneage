#include "version.h"
#include "systeminc/system.h"
#include "ohta.h"
#include "systeminc/loadsprbin.h"
#include "systeminc/t_music.h"

ACTION *MakeJiki( void );
void Jiki( ACTION *pAct );

ACTION *MakeJikiTama( int x, int y, int dx, int dy, int bmpNo );
void JikiTama( ACTION *pAct );

ACTION *MakeEnemy( void );
void Enemy( ACTION *pAct );

ACTION *MakeEnemyTama( ACTION *pOya );
void EnemyTama( ACTION *pAct );

ACTION *MakeBg( void );
void Bg( ACTION *pAct );

ACTION *MakeKen( void );
void Ken( ACTION *pAct );

ACTION *MakeGameOver( void );
void GameOver( ACTION *pAct );

/* ���k�ǩ`�� */
int bomData[] = { 21, 22, 23, 23, 22, 21 };

/* �ԙC�Υݥ��� */
ACTION *pJiki;


/* ���ĤηN� */
enum{
    TAMA_NORMAL,    /* �Ω`�ޥ� */
    TAMA_TWIN,        /* �ĥ��� */
    TAMA_3WAY,        /* ������ */
    TAMA_DIR,        /* ȫ���� */
    TAMA_END        /* �K��� */
};
    
/********************************************/
/*    �ԙC                                    */
/********************************************/

/* �ԙC��䘋���� */
typedef struct{
    int cnt,cnt2,cnt3;    /* ���å����� */
    int tamaDir;        /* ���η��� */
    int tamaDx;            /* ���Σ؉��� */
    int attackKind;        /* ���ĤηN� */
    STR_BUFFER strTestBuffer;    /* �ƥ��������� */
}JIKI;

static int No = 36000;

/* �ԙC���� *******************************************************************/
ACTION *MakeJiki( void )
{
    ACTION *pAct;
    JIKI *pYobi;
    
    /* ���������ꥹ�Ȥ˵��h */
    pAct = GetAction( PRIO_JIKI, sizeof( JIKI ) );
    if( pAct == NULL ) return NULL;
    
    /* ��䘋����Υ��ɥ쥹 */
    pYobi = ( JIKI *)pAct->pYobi;
    
    /* �g���v�� */
    pAct->func = Jiki;
    /* ��ʾ���ȶ� */
    pAct->dispPrio = DISP_PRIO_JIKI;
    // �������ж� + �ܥå�����ʾ
    pAct->atr |= ACT_ATR_HIT_BOX;
    /* ���ץ饤�ȷ��� */
    //pAct->bmpNo = 31;
    //pAct->anim_chr_no = 20001;
    // anim_tbl.h �η���
    pAct->anim_chr_no = 31555; // ���Υ���
    //pAct->anim_chr_no = 5200; // �ޥ������`����
    //pAct->anim_chr_no = 9350; // SCORE
    // ��������
    //pAct->anim_no = ANIM_ATTACK;
    pAct->anim_no = ANIM_STAND;
    pAct->anim_no = ANIM_STAND + 1;
    // ���˥�`�������( ������ )( �¤������һؤ� )
    pAct->anim_ang = 0;
    /* �ȣ� */
    pAct->hp = 100;
    /* ����λ�� */
    pAct->x = 304;
    pAct->y = 400;

    /* ������ */
//    MakeEnemy();
    /* �£����� */
    //MakeBg();
    
    /* �������� */
//    MakeKen();
    
    /* ���ĤηN� */
    //pYobi->attackKind = TAMA_DIR;
    pYobi->attackKind = TAMA_3WAY;
    /* ������ */
    pYobi->tamaDir = 1;
    pYobi->tamaDx = 1;
    /* �����`�Х�˱��� */
    pJiki = pAct;
    
    // �����Ф��L��
    pYobi->strTestBuffer.len = 10;
    // �����Ф�ɫ
    pYobi->strTestBuffer.color = 1;
    pYobi->strTestBuffer.x = 320;
    pYobi->strTestBuffer.y = 260;
    
    return pAct;
    
}

/* �ԙC�I�� *******************************************************************/
void Jiki( ACTION *pAct )
{
    JIKI *pYobi = ( JIKI *)pAct->pYobi;
    //int i;
    
    /* �����ܤ����r */
    if( pAct->damage > 0 ){
        pAct->hp -= pAct->damage;
        if( pAct->hp <= 0 ){ 
            pAct->state = ACT_STATE_DEAD;
            pYobi->cnt  = 0;
            pYobi->cnt2 = 0;
            pYobi->cnt3 = 0;
        }
        pAct->damage = 0;
    }
    // ȡ��������
//    rcX = Rnd( 0,608 );
//    rcY = Rnd( 0,400 );
//    SetRect( &Rc[ pAct->bmpNo ], rcX, rcY, rcX + 32, rcY + 32 ); 
    
    // �ޥ����Ƅ�
    //pAct->x = mouse.nowPoint.x;
    //pAct->y = mouse.nowPoint.y;

    // �ޥ����Ǐ��k��
    if( pYobi->cnt3 <= 0 ){
        // ���k��
        //if( mouse.state == MOUSE_LEFT_CRICK ){
        if( mouse.onceState & MOUSE_LEFT_CRICK ){
            // �����ե��`����ȡ��
            //GetKeyInputFocus( &pYobi->strTestBuffer );
            /* ������ */
            //MakeJikiTama( pAct->x , pAct->y -16, 0, -8, Rnd( 0, BMP_FILE - 1 ) );
            //MakeJikiTama( pAct->x , pAct->y -16, 0, -8, 1034 );
            //MakeJikiTama( pAct->x,            pAct->y -16,  0, -8, 25 );
            
            //MakeJikiTama( pAct->x,            pAct->y -16,  0, -8, Rnd( 0, BMP_FILE - 2 ) );
            
            //MakeJikiTama( pAct->x,            pAct->y -16,  0, -8, Rnd( 31554, 31557 ) ); // �ԣǣ�
            //MakeJikiTama( pAct->x,            pAct->y -16,  0, -8, Rnd( 31000, 31069 ) ); // ��󥹥��`
//            MakeJikiTama( pAct->x,            pAct->y -16,  0, -8, Rnd( 30100, 30135 ) ); // �У�
            //MakeJikiTama( pAct->x + 10, pAct->y -16, 0, -8, Rnd( 0, BMP_FILE - 1 ) );
            //MakeJikiTama( pAct->x, pAct->y -16, 0, -8, 29 );
            //MakeJikiTama( pAct->x - 10, pAct->y -16, 0, -8, 29 );
            //MakeJikiTama( pAct->x + 10, pAct->y -16, 0, -8, 29 );
            pYobi->cnt3 = 5;
#if 0
            for( i = 0; i < (int)256; i++ ){
                // ���뤯����
                Palette[i].peRed      *= 0.95;
                Palette[i].peGreen *= 0.95;
                Palette[i].peBlue  *= 0.95;
            }
            // �ѥ�åȤ��������O��
            lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
#endif
        }
        // ���Хܥ���
        if( GetAsyncKeyState( VK_MBUTTON ) ){
            //MakeJikiTama( pAct->x, pAct->y -16, 0, -8, 29 );
            //MakeJikiTama( pAct->x,            pAct->y -16,  0, -8, Rnd( 0, BMP_FILE - 1 ) );
    //        MakeJikiTama( pAct->x,            pAct->y -16,  0, -8, 32 );
            pYobi->cnt3 = 5;
            
        }
        // ���k��
        //if( mouse.onceState & MOUSE_RIGHT_CRICK ){
        if( mouse.state & MOUSE_RIGHT_CRICK ){
            // �����ե��`����ȡ��
            //GetKeyInputFocus( &MyChatBuffer );
        //if( GetAsyncKeyState( VK_RBUTTON )     & 0x8000 ){
        //    MakeJikiTama( pAct->x, pAct->y -16, 0, -8, 29 );
        // ���`�ե��������ä���
        //if( lpSurface[ 0 ] != NULL ){
        //    // ���`��
        //    lpSurface[ 0 ]->Release( );
        //    lpSurface[ 0 ] = NULL;
        //}
        //    MakeBg();
#if 0        
            MakeJikiTama( pAct->x - 10, pAct->y -16, -6, -8, Rnd( 0, BMP_FILE - 2 ) );
            MakeJikiTama( pAct->x -  5, pAct->y -16, -3, -8, Rnd( 0, BMP_FILE - 2 ) );
            MakeJikiTama( pAct->x,            pAct->y -16,  0, -8, Rnd( 0, BMP_FILE - 2 ) );
            MakeJikiTama( pAct->x +  5, pAct->y -16,  3, -8, Rnd( 0, BMP_FILE - 2 ) );
            MakeJikiTama( pAct->x + 10, pAct->y -16,  6, -8, Rnd( 0, BMP_FILE - 2 ) );
#else
    //        MakeJikiTama( pAct->x - 10, pAct->y -16, -6, -8, No );
    //        MakeJikiTama( pAct->x -  5, pAct->y -16, -3, -8, No );
    //        MakeJikiTama( pAct->x,            pAct->y -16,  0, -8, No );
    //        MakeJikiTama( pAct->x +  5, pAct->y -16,  3, -8, No );
    //        MakeJikiTama( pAct->x + 10, pAct->y -16,  6, -8, No );
#endif        
            pYobi->cnt3 = 5;
#if 0
            for( i = 0; i < (int)256; i++ ){
                // ��������
                Palette[i].peRed  *= 1.05;
                Palette[i].peGreen*= 1.05;
                Palette[i].peBlue *= 1.05;
            }
            //�ѥ�åȤ��������O��
            lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
#endif
        }
        
    }else pYobi->cnt3--;

    /* ״�B�Ƿ�� */
    switch( pAct->state ){
        /* ͨ���r */
        case ACT_STATE_ACT:
        
            /* �����Ȥε������ж� */
            //Hit( pAct, PRIO_ENEMY_TAMA );
            /* ���Ȥε������ж� */
            //Hit( pAct, PRIO_ENEMY );

            /* �Ƅ� */
            if( joy_con[ 0 ] & JOY_UP ){    /* �� */
                pAct->y -= 2;
                if( pAct->y < 0 ) pAct->y = 0;
            }
            if( joy_con[ 0 ] & JOY_DOWN ){    /* �� */
                pAct->y += 2;
                if( pAct->y > 448 ) pAct->y = 448;
            }
            if( joy_con[ 0 ] & JOY_LEFT ){    /* �� */
                pAct->x -= 2;
                if( pAct->x < 0 ) pAct->x = 0;
            }
            if( joy_con[ 0 ] & JOY_RIGHT ){    /* �� */
                pAct->x += 2;
                if( pAct->x > 608 ) pAct->x = 608;
            }
            
            // ������
            //if( joy_con[ 0 ] & JOY_B )    MakeEnemy();
            
            /* ���k�� */
            if( pYobi->cnt3 <= 0 ){ /* ���������� */
                if( joy_con[ 0 ] & JOY_A ){
                    
                    /*DeathAllAction();*/
                    /* ���ηNǷ�� */
                    switch( pYobi->attackKind ){
                        
                        case TAMA_NORMAL:
                        
                            /* ������ */
                            MakeJikiTama( pAct->x, pAct->y -16, 0, -8, 28 );
                            
                            break;
                            
                        case TAMA_TWIN:
                        
                            /* ������ */
                            MakeJikiTama( pAct->x - 10, pAct->y -16, 0, -8, 29 );
                            MakeJikiTama( pAct->x + 10, pAct->y -16, 0, -8, 29 );
                            
                            break;
                            
                        case TAMA_3WAY:
                        
                            MakeJikiTama( pAct->x - 10, pAct->y -16, -4, -8, 29 );
                            MakeJikiTama( pAct->x,            pAct->y -16,  0, -8, 29 );
                            MakeJikiTama( pAct->x + 10, pAct->y -16,  4, -8, 29 );
                            
                            break;
                            
                        case TAMA_DIR:
                        
                            MakeJikiTama( pAct->x, pAct->y -16,  pYobi->tamaDx, -8, 29 );
                            MakeJikiTama( pAct->x, pAct->y -16, -pYobi->tamaDx, -8, 29 );
                            /* ������ */
                            pYobi->tamaDx += pYobi->tamaDir;
                            if(    pYobi->tamaDx <= -10 || 10 <= pYobi->tamaDx ){
                                pYobi->tamaDir *= -1;
                            }
                            break;
                    }
                    pYobi->cnt3 = 2; /*  ���������� */
                    //pYobi->cnt3 = 0; /*  ���������� */
                }
            }else{
                if( pYobi->cnt3 > 0 ) pYobi->cnt3--;
            }
            
            /* ��� */
/*            if( ( int )pAct->x % 2 ) pAct->atr |= ACT_ATR_HIDE;
            else pAct->atr &= ~ACT_ATR_HIDE;
*/        
            /* ������ */
            pYobi->cnt++;
            if( pYobi->cnt >= 1 ){
                //MakeEnemy();
                pYobi->cnt2++;
                if( pYobi->cnt2 >= 5 ){
                    pYobi->cnt2 = 0;
                    /* ����� */
                    //pYobi->attackKind++;
                    if( pYobi->attackKind >= TAMA_END ){
                        pYobi->attackKind = 0;
                    }
                }
                pYobi->cnt = 0;
            }
            
            break;
    
        /* �����r */    
        case ACT_STATE_DEAD:
        
            if( pYobi->cnt == 0 ){ 
                pAct->bmpNo = bomData[ 0 ];
                pYobi->cnt = 1;
            }
            pYobi->cnt2++;
            if( pYobi->cnt2 >= 5 ){
                pAct->bmpNo = bomData[ pYobi->cnt ];
                pYobi->cnt++;
                pYobi->cnt2 = 0;
                if( pYobi->cnt == 6 ){
                    DeathAction( pAct );
                    MakeGameOver( );
                }
            }
            
            break;
    }
    {
        char szMoji[ 256 ];
        wsprintf( szMoji,"BmpNo = %d",No );
    }
    pattern( pAct, ANM_NOMAL_SPD, ANM_LOOP );
    StockFontBuffer2( &pYobi->strTestBuffer );
}

/********************************************/
/*    �ԙC��                                    */
/********************************************/

/* �ԙC������ *********************************************************************/
ACTION *MakeJikiTama( int x, int y, int dx, int dy, int bmpNo )
{
    ACTION *pAct;
    
    /* ���������ꥹ�Ȥ˵��h */
    pAct = GetAction( PRIO_JIKI_TAMA, 0 );
    if( pAct == NULL ) return NULL;
    
    /* �g���v�� */
    pAct->func = JikiTama;
    // anim_tbl.h �η���
    pAct->anim_chr_no = bmpNo; // ���Υ���
    // ��������
    //pAct->anim_no = ANIM_ATTACK;
    pAct->anim_no = Rnd( ANIM_ATTACK, ANIM_WALK );
    //pAct->anim_no = ANIM_WALK;
    // ���˥�`�������( ������ )( �¤������һؤ� )
    pAct->anim_ang = Rnd( 0, 7 );
    ///* ���ץ饤�ȷ��� */
    //pAct->bmpNo = bmpNo;
    /* ��ʾ���ȶ� */
    pAct->dispPrio = 2;
    /* �ȣ� */
    pAct->hp = 10000;
    /* ����λ�� */
    pAct->x = x - dx;
    pAct->y = y - dy;
    /* ���� */
    pAct->dx = dx / 2;
    pAct->dy = dy / 2;
    
    return pAct;
}

/* �ԙC���I�� *********************************************************************/
void JikiTama( ACTION *pAct )
{

    /* �����ܤ����r */
    if( pAct->damage > 0 ){
        pAct->hp -= pAct->damage;
        if( pAct->hp <= 0 ){ 
            pAct->state = ACT_STATE_DEAD;
        }
        pAct->damage = 0;
    }

    /* ״�B�Ƿ�� */
    switch( pAct->state ){
        /* ͨ���r */
        case ACT_STATE_ACT:

            /* ���Ȥε������ж� */
            //Hit( pAct, PRIO_ENEMY );

            /* �Ƅ� */
            pAct->x += pAct->dx;
            pAct->y += pAct->dy;
    
            /* ��ߥå� */
            if(    pAct->y <= -32 ){ 
                pAct->y = -32;
                MakeEnemyTama( pAct );
                DeathAction( pAct );
            }else
            if( lpDraw->ySize <= pAct->y ){ 
                pAct->y = lpDraw->ySize;
                DeathAction( pAct );
            }
            
            if(    pAct->x <= -32 ){
                pAct->x = -32;
                DeathAction( pAct );
            }else
            if( lpDraw->xSize <= pAct->x ){ 
                pAct->x = lpDraw->xSize;
                DeathAction( pAct );
            }

            break;
        
        /* �����r */    
        case ACT_STATE_DEAD:
        
            DeathAction( pAct );
            
            break;
    }
    
    
    // ���˥�`�����
    pattern( pAct, ANM_NOMAL_SPD, ANM_LOOP );
}


/********************************************/
/*    ��                                        */
/********************************************/

/* ����䘋���� */
typedef struct{
    int cnt,cnt2,cnt3,animCnt;    /* ���å����� */
}ENEMY;

/* ������ *********************************************************************/
ACTION *MakeEnemy( void )
{
    ACTION *pAct;
    ENEMY *pYobi;
    short bmpTbl[] = { 1, 3, 5, 7, 9, 29 };
    
    /* ���������ꥹ�Ȥ˵��h */
    pAct = GetAction( PRIO_ENEMY, sizeof( ENEMY ) );
    if( pAct == NULL ) return NULL;
    
    /* ��䘋����Υ��ɥ쥹 */
    pYobi = ( ENEMY *)pAct->pYobi;
    
    /* �g���v�� */
    pAct->func = Enemy;
    /* ���ץ饤�ȷ��� */
    pAct->bmpNo = bmpTbl[ ( int )Rnd( 0, 4 ) ];
    //pAct->bmpNo = Rnd( 1, 10 );
    /* ��ʾ���ȶ� */
    pAct->dispPrio = 1;
    /* �ȣ� */
    pAct->hp = 10;//Rnd( 10, 50 );
    /* ����λ�å����� */
//    pAct->x = Rnd( 0, 608 );
    pAct->x = Rnd( 0, lpDraw->xSize );
    ATR_CRS(pAct) = 16;
    ATR_SPD(pAct) = Rnd( 28, 36 );
    pAct->dx = 0;

    pAct->y = 0;
    /* ���֥����� */
    pAct->dx = Rnd( -3, 3 );
    pAct->dy = Rnd( 1, 3 );
    
    return pAct;
}

/* �����I�� *********************************************************************/
void enemy_missile( ACTION *a0 )
{
    gemini(a0);        //�Ƅ�
    //������ʤ�
    if(ATR_H_POS(a0)>=lpDraw->xSize || ATR_H_POS(a0)<-32 || ATR_V_POS(a0)>lpDraw->ySize || ATR_V_POS(a0)<-32)
        DeathAction( a0 );        //�K��
}

/* ���I�� *********************************************************************/
void Enemy( ACTION *pAct )
{
    ENEMY *pYobi = ( ENEMY *)pAct->pYobi;

    /* �����ܤ����r */
    if( pAct->damage > 0 ){
        pAct->hp -= pAct->damage;
        if( pAct->hp <= 0 ){ 
            pAct->state = ACT_STATE_DEAD;
            pYobi->cnt  = 0;
            pYobi->cnt2 = 0;
            pYobi->cnt3 = 0;
        }
        pAct->damage = 0;
    }

    /* ״�B�Ƿ�� */
    switch( pAct->state ){
        /* ͨ���r */
        case ACT_STATE_ACT:
        
            /* �Ƅ� */
//            pAct->x += pAct->dx;
//            pAct->y += pAct->dy;

            /* ��ߥåȥ����å� */
//            if( pAct->x < 0 ){ 
//                pAct->x = 0;
//                pAct->dx *= -1;
//            }
//            else
//            if( pAct->x > lpDraw->xSize ){ 
//                pAct->x = lpDraw->xSize;
//                pAct->dx *= -1;
//            }
//            if( pAct->y > lpDraw->ySize ){ 
//                pAct->y = -32;
//            }

            pAct->dx++;        //������z�ߤˣ��إ��ԩ`�ɉ������
            pAct->dx&=1;    //
            gemini(pAct);        //�Ƅ�
            if(ATR_CRS(pAct)==0){        //���Ƅ��Фʤ�
                if( pAct->y < -32 ){            //�϶ˤʤ�
                    ATR_CRS(pAct)=16;            //���Ƅ��_ʼ
                    ATR_SPD(pAct) = Rnd( 28, 36 );
                    pAct->x = Rnd( 0, lpDraw->xSize );    //
                } else {
                    if(pAct->dx)
                        ATR_SPD(pAct)++;        //���ԩ`�ɥ��å�
                }
            } else {
                if(pAct->dx){
                    ATR_SPD(pAct)--;        //���ԩ`�ɥ�����
                    if(!ATR_SPD(pAct)){        //ֹͣ�����ʤ�
                        ATR_CRS(pAct)=0;        //���Ƅ��_ʼ
                        ACTION *a1;        //�ߥ�����k��
                        int d1,d2;
                        /* ���������ꥹ�Ȥ˵��h */
                        a1 = GetAction( PRIO_ENEMY_TAMA, 0 );
                        if( a1 == NULL ) break;
                        /* �g���v�� */
                        a1->func = enemy_missile;
                        /* ���ץ饤�ȷ��� */
                        a1->bmpNo = No++;
                        /* ��ʾ���ȶ� */
                        a1->dispPrio = 3;
                        /* ����λ�� */
                        ATR_H_POS(a1) = ATR_H_POS(pAct)+16;
                        ATR_V_POS(a1) = ATR_V_POS(pAct)+16;
                        d1 = pJiki->x;            //�ԙC�η��򥻥å�
                        d2 = pJiki->y;            //
                        radar(a1, &d1, &d2);    //
                        ATR_CRS(a1)=d1;            //
                        ATR_SPD(a1)=16;            //�Ƅ��ٶ�
                        play_se( 2, ATR_H_POS(a1), ATR_V_POS(a1) );        //�ߥ�����k����
                    }
                }
            }
            // ���˥�`�����
/*            pYobi->cnt3 += pAct->dy;
            if( pYobi->cnt3 >= 20 ){ 
                pYobi->cnt3 = 0;
                pYobi->animCnt = pYobi->animCnt * ( -2 ) + 1;
                pAct->bmpNo += pYobi->animCnt;
            }
*/
/*            
            pAct->cnt++;
            if( pAct->cnt >= 300 ){
                pAct->state = ACT_STATE_DEAD;
                pAct->cnt = 0;
            }
*/            
            //if( Rnd( 0, 300 ) == 0 ) MakeEnemyTama( pAct );
            //MakeEnemyTama( pAct );
            
            break;
        
        /* �����r */    
        case ACT_STATE_DEAD:
        
            if( pYobi->cnt == 0 ){ 
                pAct->bmpNo = bomData[ 0 ];
                pYobi->cnt = 1;
            }
            /* ���k���˥�`����� */
            pYobi->cnt2++;
            if( pYobi->cnt2 >= 5 ){
                pAct->bmpNo = bomData[ pYobi->cnt ];
                pYobi->cnt++;
                pYobi->cnt2 = 0;
                /* �K��ä������� */
                if( pYobi->cnt == 6 ){
                    DeathAction( pAct );
                }
            }
            
            break;
    }

}

/********************************************/
/*    ����                                    */
/********************************************/

/* �������� *********************************************************************/
ACTION *MakeEnemyTama( ACTION *pOya )
{
    ACTION *pAct;
    //float dir;
    
    /* ���������ꥹ�Ȥ˵��h */
    pAct = GetAction( PRIO_ENEMY_TAMA, 0 );
    if( pAct == NULL ) return NULL;
    
    /* �g���v�� */
    pAct->func = EnemyTama;
    /* ���ץ饤�ȷ��� */
    pAct->anim_chr_no = Rnd( 31554, 31556 );        //���饤��
    // �������ж� + �ܥå�����ʾ
    pAct->atr |= ACT_ATR_HIT_BOX;
    //pAct->anim_no = Rnd( ANIM_ATTACK, ANIM_SATTACK );
    //pAct->anim_no = ANIM_ATTACK;
    pAct->anim_no = ANIM_WALK;
    /* ��ʾ���ȶ� */
    pAct->dispPrio = DISP_PRIO_JIKI;
    /* �ȣ� */
    pAct->hp = 0;
    /* ����λ�� */
    //pAct->x = pOya->x + 16;
    //pAct->y = pOya->y + 32 ;
    pAct->x = Rnd( 100, 500 );
    pAct->y = Rnd( 100, 400 );
    
    // �Ƅӷ���    
    radar2( pAct, pJiki->x, pJiki->y, 1 );
    //�Ƅ��ٶ�
    pAct->spd = 16;            
    
    // ���˥�`�����
    pattern( pAct, ANM_NOMAL_SPD, ANM_LOOP );
    return pAct;
}

/* �����I�� *********************************************************************/
void EnemyTama( ACTION *pAct )
{

    /* �����ܤ����r */
    if( pAct->damage > 0 ){
        pAct->hp -= pAct->damage;
        if( pAct->hp <= 0 ){ 
            pAct->state = ACT_STATE_DEAD;
        }
        pAct->damage = 0;
    }

    /* ״�B�Ƿ�� */
    switch( pAct->state ){
        /* ͨ���r */
        case ACT_STATE_ACT:
            
            // �ޥ������`���뤬�ҥåȤ��Ƥ�����
            if( pAct->hitDispNo == HitDispNo ){
                // �󥯥�å���Ѻ����Ƥ���r
                if( mouse.state & MOUSE_LEFT_CRICK ){
                    // �ɥ�å��Ƅ�
                    pAct->x = mouse.nowPoint.x;
                    pAct->y = mouse.nowPoint.y;
                }
                // �󥯥�å���Ѻ����Ƥ���r
                if( mouse.state & MOUSE_RIGHT_CRICK ){
                    // �ɥ�å��Ƅ�
                    DeathAction( pAct );
                }
                
            }    
            /* �Ƅ� */
            //pAct->x += pAct->dx;
            //pAct->y += pAct->dy;
            //gemini( pAct );        //�Ƅ�
#if 0    
            /* ��ߥå� */
            if(    pAct->y <= 0 ){
                pAct->y = 0;
                DeathAction( pAct );
            }else
            if(    pAct->y >= 481 ){ 
                DeathAction( pAct );
            }
            if(    pAct->x <= -8 ){
                DeathAction( pAct );
            }else
            if( lpDraw->xSize <= pAct->x ){ 
                DeathAction( pAct );
            }
#endif            
            break;
        
        /* �����r */    
        case ACT_STATE_DEAD:
            DeathAction( pAct );
            break;
    }
    // �Ƅӷ���    
    radar2( pAct, pJiki->x, pJiki->y, Rnd( 0, 10 ) );
    // ���˥�`�����
    pattern( pAct, ANM_NOMAL_SPD, ANM_LOOP );
}

/* ���`�४�`�Щ`���� *******************************************************************/
ACTION *MakeGameOver( void )
{
    ACTION *pAct;
    
    /* ���������ꥹ�Ȥ˵��h */
    pAct = GetAction( PRIO_GAME_OVER, 0 );
    if( pAct == NULL ) return NULL;
    
    /* �g���v�� */
    pAct->func = GameOver;
    
    // �Ǳ�ʾ
    pAct->atr |= ACT_ATR_HIDE;
    
    /* ���ץ饤�ȷ��� */
    pAct->bmpNo = 0;
//    pAct->bmpNo = 30;
    /* �ȣ� */
    pAct->hp = 100;
    /* ����λ�� */
    pAct->x = 250;
    pAct->y = 200;

    return pAct;
    
}

/* ���`�४�`�Щ`�I�� *******************************************************************/
void GameOver( ACTION *pAct )
{
    //char szMoji[128];
    
    // GAME OVER ��ʾ
    
}

/* �£����� *********************************************************************/
ACTION *MakeBg( void )
{
    ACTION *pAct;
    
    /* ���������ꥹ�Ȥ˵��h */
    pAct = GetAction( PRIO_BG, NULL );
    if( pAct == NULL ) return NULL;
    
    /* �g���v�� */
    pAct->func = Bg;
    
    /* ���ץ饤�ȷ��� */
    //pAct->bmpNo = 30;
//    pAct->bmpNo = 44;
    pAct->anim_chr_no = 9376;
    /* ��ʾ���ȶ� */
    pAct->dispPrio = 0;
    /* ����λ�� */
    pAct->x = 320;
    pAct->y = 240;

    // ȡ��������
    //pYobi->rcX = 0;
    //pYobi->rcY = lpDraw->ySize;
    //SetRect( &Rc[ pAct->bmpNo ], pYobi->rcX, pYobi->rcY, pYobi->rcX + lpDraw->xSize, pYobi->rcY + lpDraw->ySize ); 

    return pAct;
}
    
/* �£ǄI�� *********************************************************************/
void Bg( ACTION *pAct )
{
    // ȡ��������
    // ȡ��������
    //pAct->y++;
    //if( pAct->y >= 240 ) pAct->y = -240; 
    pattern( pAct, ANM_NOMAL_SPD, ANM_LOOP );        //���˥�`�����I��
    //StockDispBuffer( pAct->x, pAct->y+lpDraw->ySize, 0, pAct->anim_chr_no, 0 );
    //StockDispBuffer( pAct->x, pAct->y, 0, pAct->anim_chr_no, 0 );
    //StockDispBuffer( pAct->x, pAct->y+lpDraw->ySize, 0, pAct->anim_chr_no, 0);
}


/* �������� *********************************************************************/
ACTION *MakeKen( void )
{
    ACTION *pAct;
    
    /* ���������ꥹ�Ȥ˵��h */
    pAct = GetAction( PRIO_BG, NULL );
    if( pAct == NULL ) return NULL;
    
    /* �g���v�� */
    pAct->func = Ken;
    
    /* ���ץ饤�ȷ��� */
    pAct->bmpNo = 33;
    /* ��ʾ���ȶ� */
    pAct->dispPrio = 100;
    /* ����λ�� */
    pAct->x = 100;
    pAct->y = 100;
    
    // ȡ��������
    //pYobi->rcX = 0;
    //pYobi->rcY = lpDraw->ySize;
    //SetRect( &Rc[ pAct->bmpNo ], pYobi->rcX, pYobi->rcY, pYobi->rcX + lpDraw->xSize, pYobi->rcY + lpDraw->ySize ); 

    return pAct;
}
    
/* ����I�� *********************************************************************/
void Ken( ACTION *pAct )
{
    int anim[] = {     0,0,0,0,
                    1,1,1,1,
                    2,2,2,2,
                    3,3,3,3,
                    2,2,2,2,
                    1,1,1,1 
                    };
}

