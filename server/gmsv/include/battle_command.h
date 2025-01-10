#ifndef __BATTLE_COMMAND_H__
#define __BATTLE_COMMAND_H__

void BattleCommandDispach(int fd, char *command);

void BattleEncountOut(int char_index);

#define BATTLE_CommandSend(char_index, pszCommand) _BATTLE_CommandSend(char_index, pszCommand, __FILE__, __LINE__)
BOOL _BATTLE_CommandSend(int char_index, char *pszCommand, char *file, int line);
BOOL BATTLE_MakeCharaString(int battle_index, char *pszCommand, int	size);

void BATTLE_CharSendAll(int battle_index);
void BATTLE_CharSendOne(int battle_index, int mychar_index);

BOOL BATTLE_PetDefaultCommand(int petindex);
void BATTLE_ActSettingSend(int battle_index);

#endif
