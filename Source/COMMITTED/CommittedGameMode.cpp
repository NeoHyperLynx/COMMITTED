#include "CommittedGameMode.h"
#include "Combat/CommittedFighterCharacter.h"

ACommittedGameMode::ACommittedGameMode()
{
    DefaultPawnClass = ACommittedFighterCharacter::StaticClass();
}
