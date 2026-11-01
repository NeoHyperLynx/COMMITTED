#include "CommittedFighterRegistry.h"
#include "GameplayTagsManager.h"

TArray<FCommittedFighterDefinition> UCommittedFighterRegistry::GetVerified15Fighters()
{
    TArray<FCommittedFighterDefinition> Roster;

    auto CreateFighter = [](FName ID, const FString& Name, const FString& Tradition, const FString& Region,
                            EFighterArchetype Archetype, EFighterExchangeIdentity Identity,
                            float PostureMax, float PostureRegen, float FlowGain,
                            const FString& SigMoveName, float SigDamage, float SigStartup)
    {
        FCommittedFighterDefinition Def;
        Def.RowID = ID;
        Def.FighterName = FText::FromString(Name);
        Def.TraditionName = FText::FromString(Tradition);
        Def.OriginRegion = FText::FromString(Region);
        Def.Archetype = Archetype;
        Def.ExchangeIdentity = Identity;
        Def.PostureMax = PostureMax;
        Def.PostureRegenRate = PostureRegen;
        Def.FlowGainRate = FlowGain;
        Def.FlowMax = 100.f;

        Def.LightAttack.Name = FName(*(Tradition + TEXT(" Quick Strike")));
        Def.LightAttack.StartupSeconds = 0.16f;
        Def.LightAttack.ActiveSeconds = 0.14f;
        Def.LightAttack.RecoverySeconds = 0.26f;
        Def.LightAttack.Damage = 16.f;
        Def.LightAttack.PostureDamage = 14.f;

        Def.HeavyAttack.Name = FName(*(Tradition + TEXT(" Heavy Impact")));
        Def.HeavyAttack.StartupSeconds = 0.38f;
        Def.HeavyAttack.ActiveSeconds = 0.16f;
        Def.HeavyAttack.RecoverySeconds = 0.44f;
        Def.HeavyAttack.Damage = 28.f;
        Def.HeavyAttack.PostureDamage = 32.f;

        Def.SignatureAttack.Name = FName(*SigMoveName);
        Def.SignatureAttack.StartupSeconds = SigStartup;
        Def.SignatureAttack.ActiveSeconds = 0.20f;
        Def.SignatureAttack.RecoverySeconds = 0.50f;
        Def.SignatureAttack.Damage = SigDamage;
        Def.SignatureAttack.PostureDamage = SigDamage * 1.5f;

        return Def;
    };

    // 1. Muay Thai (Thailand - Striker)
    Roster.Add(CreateFighter(TEXT("MuayThai"), TEXT("Pranom"), TEXT("Muay Thai"), TEXT("Thailand"),
        EFighterArchetype::Striker, EFighterExchangeIdentity::BareHandStriker, 95.f, 18.f, 14.f,
        TEXT("Flying Clinch Knee"), 35.f, 0.28f));

    // 2. Karate Shotokan (Japan - Striker)
    Roster.Add(CreateFighter(TEXT("Karate"), TEXT("Ryuhei"), TEXT("Karate (Shotokan)"), TEXT("Japan"),
        EFighterArchetype::Striker, EFighterExchangeIdentity::BareHandStriker, 100.f, 18.f, 12.f,
        TEXT("Gyaku-Zuki Reverse Punch"), 32.f, 0.24f));

    // 3. Taekwondo (South Korea - Striker)
    Roster.Add(CreateFighter(TEXT("Taekwondo"), TEXT("Min-jun"), TEXT("Taekwondo"), TEXT("South Korea"),
        EFighterArchetype::Striker, EFighterExchangeIdentity::BareHandStriker, 85.f, 16.f, 15.f,
        TEXT("Tornado 540 Roundhouse"), 38.f, 0.32f));

    // 4. Cuban Boxing (Cuba - Striker)
    Roster.Add(CreateFighter(TEXT("Boxing"), TEXT("Maceo"), TEXT("Cuban Boxing"), TEXT("Cuba"),
        EFighterArchetype::Striker, EFighterExchangeIdentity::BareHandStriker, 105.f, 20.f, 13.f,
        TEXT("Cross Counter Lead Hook"), 30.f, 0.20f));

    // 5. Judo (Japan - Grappler)
    Roster.Add(CreateFighter(TEXT("Judo"), TEXT("Kenji"), TEXT("Judo"), TEXT("Japan"),
        EFighterArchetype::Grappler, EFighterExchangeIdentity::Grappler, 115.f, 22.f, 9.f,
        TEXT("Ippon Seoi Nage"), 34.f, 0.30f));

    // 6. Laamb (Senegal - Grappler)
    Roster.Add(CreateFighter(TEXT("Laamb"), TEXT("Mamadou"), TEXT("Laamb Wrestling"), TEXT("Senegal"),
        EFighterArchetype::Grappler, EFighterExchangeIdentity::Grappler, 125.f, 24.f, 8.f,
        TEXT("Mbapat Sweep Slam"), 40.f, 0.36f));

    // 7. Sambo (Eastern Europe - Grappler)
    Roster.Add(CreateFighter(TEXT("Sambo"), TEXT("Viktor"), TEXT("Combat Sambo"), TEXT("Eastern Europe"),
        EFighterArchetype::Grappler, EFighterExchangeIdentity::Grappler, 110.f, 20.f, 10.f,
        TEXT("Rolling Kneebar Leg Lock"), 35.f, 0.28f));

    // 8. Arnis / Kali (Philippines - Weapon)
    Roster.Add(CreateFighter(TEXT("Arnis"), TEXT("Bayani"), TEXT("Arnis / Kali"), TEXT("Philippines"),
        EFighterArchetype::Weapon, EFighterExchangeIdentity::StickWeapon, 120.f, 15.f, 7.f,
        TEXT("Sinawali Double-Stick Flurry"), 36.f, 0.26f));

    // 9. Tahtib (Egypt - Weapon)
    Roster.Add(CreateFighter(TEXT("Tahtib"), TEXT("Tarek"), TEXT("Tahtib Stick"), TEXT("Egypt"),
        EFighterArchetype::Weapon, EFighterExchangeIdentity::StickWeapon, 115.f, 16.f, 8.f,
        TEXT("Asaya Overhead Strike"), 38.f, 0.34f));

    // 10. Nguni Stick Fighting (Southern Africa - Weapon)
    Roster.Add(CreateFighter(TEXT("Nguni"), TEXT("Sipho"), TEXT("Nguni Stick Fighting"), TEXT("Southern Africa"),
        EFighterArchetype::Weapon, EFighterExchangeIdentity::StickWeapon, 120.f, 17.f, 8.f,
        TEXT("Induku Shield-Bash & Crack"), 37.f, 0.30f));

    // 11. Khridoli (Georgia - Weapon)
    Roster.Add(CreateFighter(TEXT("Khridoli"), TEXT("Irakli"), TEXT("Khridoli"), TEXT("Georgia"),
        EFighterArchetype::Weapon, EFighterExchangeIdentity::StickWeapon, 110.f, 16.f, 9.f,
        TEXT("Sabatoba Saber Blade Slash"), 36.f, 0.28f));

    // 12. Capoeira (Brazil - Kicking/Flow)
    Roster.Add(CreateFighter(TEXT("Capoeira"), TEXT("Tiago"), TEXT("Capoeira"), TEXT("Brazil"),
        EFighterArchetype::Hybrid, EFighterExchangeIdentity::KickingFlow, 80.f, 14.f, 18.f,
        TEXT("Meia Lua de Compasso"), 34.f, 0.22f));

    // 13. Savate (France - Kicking/Flow)
    Roster.Add(CreateFighter(TEXT("Savate"), TEXT("Julien"), TEXT("Savate Boxe Française"), TEXT("France"),
        EFighterArchetype::Striker, EFighterExchangeIdentity::KickingFlow, 85.f, 16.f, 16.f,
        TEXT("Fouetté High Whip Kick"), 33.f, 0.22f));

    // 14. Wing Chun (China - Kicking/Flow)
    Roster.Add(CreateFighter(TEXT("WingChun"), TEXT("Yip"), TEXT("Wing Chun"), TEXT("China"),
        EFighterArchetype::Hybrid, EFighterExchangeIdentity::KickingFlow, 90.f, 18.f, 15.f,
        TEXT("Chain Punch Centerline Blitz"), 30.f, 0.18f));

    // 15. Pencak Silat (Indonesia - Kicking/Flow)
    Roster.Add(CreateFighter(TEXT("Silat"), TEXT("Agung"), TEXT("Pencak Silat"), TEXT("Indonesia"),
        EFighterArchetype::Hybrid, EFighterExchangeIdentity::KickingFlow, 85.f, 17.f, 17.f,
        TEXT("Harimau Low Sweep & Snap"), 35.f, 0.24f));

    return Roster;
}

bool UCommittedFighterRegistry::GetFighterDefinition(FName RowID, FCommittedFighterDefinition& OutDef)
{
    const TArray<FCommittedFighterDefinition> Fighters = GetVerified15Fighters();
    for (const FCommittedFighterDefinition& F : Fighters)
    {
        if (F.RowID == RowID)
        {
            OutDef = F;
            return true;
        }
    }
    return false;
}
