#include "DevotedGameMode.h"
#include "Combat/DevotedFighterCharacter.h"

ADevotedGameMode::ADevotedGameMode()
{
    DefaultPawnClass = ADevotedFighterCharacter::StaticClass();
}
