#include "SaveDetector.h"
#include "../GamePak.h"
#include <iostream>


static std::unordered_map<std::string, SaveType> SaveTypeIds;
static std::unordered_map<std::string, SaveType> SaveTypeLookup;

SaveType detectSavetype(GamePak& pak)
{
    SaveType stype = lookupSavetype(pak);
    if (stype == SaveType::UNKNOWN) {
        printf("(Save Detector) Could not find ROM savetype in savetype database\n");
        return SaveType::NONE;
    }
    return stype;
}

SaveType lookupSavetype(GamePak& pak)
{
    std::string game_code = pak.header.game_code;
    game_code.pop_back(); //remove the language byte, dont need it for the table
    SaveType stype = SaveTypeLookup[game_code];

    return stype;
}

void setupSaveDatabase()
{
	SaveTypeIds = {
		{"EEPROM_V", SaveType::EEPROM_8k},
		{"SRAM_V",  SaveType::SRAM_256K},
		{"SRAM_F_V", SaveType::SRAM_256K},
		{"FLASH_V", SaveType::Flash_512k_SST},
		{"FLASH512_V", SaveType::Flash_512k_SST},
        {"FLASH1M_V",  SaveType::Flash_1M_Macronix}
    };


    // lut derived from https://github.com/profi200/open_agb_firm/issues/9
    SaveTypeLookup = {
            {"BJB" , SaveType::EEPROM_4k}, // EEPROM_V122     007 - Everything or Nothing
            {"BFB" , SaveType::SRAM_256K}, // SRAM_V113       2 Disney Games - Disney Sports Skateboarding + Football
            {"BLQ" , SaveType::EEPROM_4k}, // EEPROM_V124     2 Disney Games - Lilo & Stitch 2 + Peter Pan
            {"BQJ" , SaveType::NONE}, // NONE}            2 Game Pack - Hot Wheels Stunt Track + World Race
            {"BB4" , SaveType::NONE}, // NONE}            2 Game Pack! - Matchbox Missions
            {"BUQ" , SaveType::NONE}, // NONE}            2 Game Pack! Uno + Skip-Bo
            {"BX6" , SaveType::NONE}, // NONE}            2 Games in 1 - Battle for Bikini Bottom + Breakin' Da Rules
            {"BU2" , SaveType::EEPROM_4k}, // EEPROM_V124     2 Games in 1 - Battle for Bikini Bottom + Nicktoons Freeze Frame Frenzy
            {"BL5" , SaveType::NONE}, // NONE}            2 Games in 1 - Bionicle + Knights' Kingdom
            {"BWB" , SaveType::EEPROM_4k}, // EEPROM_V122     2 Games in 1 - Brother Bear + Disney Princess
            {"BLB" , SaveType::EEPROM_4k}, // EEPROM_V122     2 Games in 1 - Brother Bear + The Lion King
            {"BW2" , SaveType::NONE}, // NONE}            2 Games in 1 - Cartoon Network - Block Party + Speedway
            {"BW9" , SaveType::Flash_512k_Panasonic}, // FLASH_V133      2 Games in 1 - Columns Crown + ChuChu Rocket!
            {"BLP" , SaveType::EEPROM_4k}, // EEPROM_V124     2 Games in 1 - Disney Princess + The Lion King
            {"B2E" , SaveType::NONE}, // NONE}            2 Games in 1 - Dora the Explorer - Pirate Pig's Treasure + Super Star Adventures
            {"BZP" , SaveType::EEPROM_4k}, // EEPROM_V124     2 Games in 1 - Dr. Mario + Puzzle League
            {"BUF" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     2 Games in 1 - Dragon Ball Z - Buu's Fury + Dragon Ball GT - Transformation
            {"BFW" , SaveType::NONE}, // NONE}            2 Games in 1 - Finding Nemo + Finding Nemo - The Continuing Adventures
            {"BDZ" , SaveType::NONE}, // NONE}            2 Games in 1 - Finding Nemo + Monsters, Inc.
            {"BIN" , SaveType::NONE}, // NONE}            2 Games in 1 - Finding Nemo + The Incredibles
            {"BWC" , SaveType::NONE}, // NONE}            2 Games in 1 - Golden Nugget Casino - Texas Hold'em Poker
            {"BHZ" , SaveType::NONE}, // NONE}            2 Games in 1 - Hot Wheels - World Race + Velocity X
            {"BLD" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     2 Games in 1 - Lizzie McGuire - Disney Princess
            {"BAR" , SaveType::EEPROM_4k}, // EEPROM_V122     2 Games in 1 - Moto GP - GT 3 Advance
            {"BRZ" , SaveType::NONE}, // NONE}            2 Games in 1 - Power Rangers Time Force + Ninja Storm
            {"BWQ" , SaveType::NONE}, // NONE}            2 Games in 1 - Quad Desert Fury + Monster Trucks
            {"BPU" , SaveType::EEPROM_4k}, // EEPROM_V124     2 Games in 1 - Scooby-Doo + Scooby-Doo 2
            {"BCV" , SaveType::EEPROM_4k}, // EEPROM_V122     2 Games in 1 - Scooby-Doo and the Cyber Chase + Mystery Mayhem
            {"BW3" , SaveType::Flash_512k_Panasonic}, // FLASH_V133      2 Games in 1 - Sonic Advance + Chu Chu Rocket
            {"BW7" , SaveType::Flash_512k_Panasonic}, // FLASH_V133      2 Games in 1 - Sonic Battle + ChuChu Rocket!
            {"BW4" , SaveType::Flash_512k_Panasonic}, // FLASH_V133      2 Games in 1 - Sonic Battle + Sonic Advance
            {"BW8" , SaveType::Flash_512k_Panasonic}, // FLASH_V133      2 Games in 1 - Sonic Pinball + Columns Crown
            {"BW6" , SaveType::Flash_512k_Panasonic}, // FLASH_V133      2 Games in 1 - Sonic Pinball Party + Sonic Battle
            {"BSZ" , SaveType::NONE}, // NONE}            2 Games in 1 - SpongeBob SquarePants - Supersponge + Battle for Bikini Bottom
            {"BDF" , SaveType::NONE}, // NONE}            2 Games in 1 - SpongeBob SquarePants - Supersponge + Revenge of the Flying Dutchman
            {"BRS" , SaveType::NONE}, // NONE}            2 Games in 1 - SpongeBob SquarePants - Supersponge + Rugrats - Go Wild
            {"BBJ" , SaveType::NONE}, // NONE}            2 Games in 1 - SpongeBob SquarePants + Jimmy Neutron
            {"BNE" , SaveType::NONE}, // NONE}            2 Games in 1 - The Incredibles + Finding Nemo - The Continuing Adventure
            {"B2B" , SaveType::EEPROM_4k}, // EEPROM_V124     2 Games in 1 - The SpongeBob SquarePants Movie + Freeze Frame Frenzy
            {"B2A" , SaveType::EEPROM_4k}, // EEPROM_V124     2 in 1 - Asterix & Obelix - PAF! Them All! + XXL
            {"BLF" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     2 in 1 - Dragon Ball Z 1 and 2
            {"B94" , SaveType::EEPROM_8k}, // EEPROM_V124     2 in 1 - Pferd & Pony - Mein Pferdehof + Lass Uns Reiten 2
            {"BX2" , SaveType::EEPROM_4k}, // EEPROM_V124     2 in 1 - Spider-Man Mysterio's Menace & X2 Wolverine's Revenge
            {"BX4" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     2 in 1 - Tony Hawk's Underground + Kelly Slater's Pro Surfer
            {"BCS" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     2 in 1 - V-Rally 3 - Stuntman
            {"BXH" , SaveType::EEPROM_4k}, // EEPROM_V124     2 in 1 Fun Pack - Madagascar - Operation Penguin + Shrek 2
            {"BXG" , SaveType::EEPROM_4k}, // EEPROM_V124     2 in 1 Fun Pack - Madagascar + Shrek 2
            {"BS7" , SaveType::EEPROM_4k}, // EEPROM_V124     2 in 1 Game Pack - Shark Tale + Shrek 2
            {"BX3" , SaveType::EEPROM_4k_alt}, // EEPROM_V125     2 in 1 GamePack - Spider-Man + Spider-Man 2
            {"BT5" , SaveType::NONE}, // NONE}            2 Jeux en 1 - Titeuf - Ze Gag Machine + Mega Compet
            {"BC4" , SaveType::NONE}, // NONE}            3 Game Pack - Candy Land + Chutes and Ladders + Memory
            {"B3O" , SaveType::NONE}, // NONE}            3 Game Pack - Mouse Trap + Simon + Operation
            {"B3U" , SaveType::NONE}, // NONE}            3 Game Pack - The Game of Life + Yahtzee + Payday
            {"BXC" , SaveType::NONE}, // NONE}            3 Game Pack! - Ker Plunk! + Toss Across + Tip It
            {"B44" , SaveType::EEPROM_4k}, // EEPROM_V122     3 Games in 1 - Rugrats, SpongeBob, Tak
            {"BRQ" , SaveType::NONE}, // NONE}            3 Games in One - Majesco's Rec Room Challenge
            {"B3N" , SaveType::NONE}, // NONE}            3 Games in One - Majesco's Sports Pack
            {"BI4" , SaveType::SRAM_256K}, // SRAM_V113       4 Games on One Game Pak - GT Advance - GT Advance 2 - GT Advance 3 - Moto GP
            {"BI7" , SaveType::SRAM_256K}, // SRAM_V113       4 Games on One Game Pak - Nickelodeon
            {"BI6" , SaveType::SRAM_256K}, // SRAM_V113       4 Games on One Game Pak (Nickelodeon Movies)
            {"A3Q" , SaveType::NONE}, // NONE}            A Sound of Thunder
            {"BAE" , SaveType::NONE}, // NONE}            Ace Combat Advance
            {"ALX" , SaveType::EEPROM_4k}, // EEPROM_V122     Ace Lightning
            {"BAC" , SaveType::NONE}, // NONE}            Action Man - Robot Atak
            {"BAV" , SaveType::EEPROM_4k}, // EEPROM_V122     Activision Anthology
            {"AG7" , SaveType::SRAM_256K}, // SRAM_V110       Advance GTA
            {"BGC" , SaveType::EEPROM_4k}, // EEPROM_V124     Advance Guardian Heroes
            {"BAG" , SaveType::EEPROM_4k}, // EEPROM_V124     Advance Guardian Heroes
            {"AR7" , SaveType::SRAM_256K}, // SRAM_V112       Advance Rally
            {"AWR" , SaveType::Flash_512k_Atmel}, // FLASH_V121      Advance Wars
            {"AW2" , SaveType::Flash_512k_SST}, // FLASH_V126      Advance Wars 2 - Black Hole Rising
            {"ADE" , SaveType::SRAM_256K}, // SRAM_V112       Adventure of Tokyo Disney Sea
            {"AAO" , SaveType::EEPROM_4k}, // EEPROM_V122     Aero the Acro-Bat - Rascal Rival Revenge
            {"ACE" , SaveType::NONE}, // NONE}            Agassi Tennis Generation
            {"BHQ" , SaveType::NONE}, // NONE}            Agent Hugo - Roborumble
            {"AIL" , SaveType::NONE}, // NONE}            Aggressive Inline
            {"AAK" , SaveType::EEPROM_4k}, // EEPROM_V122     Air Force Delta Storm
            {"BAZ" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Akachan Doubutsu Sono
            {"BZW" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Akagi
            {"BAD" , SaveType::EEPROM_4k}, // EEPROM_V122     Aladdin
            {"AJ6" , SaveType::EEPROM_4k}, // EEPROM_V122     Aladdin
            {"BAB" , SaveType::EEPROM_4k}, // EEPROM_V124     Aleck Bordon Adventure - Tower & Shaft Advance
            {"ATF" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Alex Ferguson's Player Manager 2002
            {"BAW" , SaveType::EEPROM_4k}, // EEPROM_V124     Alex Rider - Stormbreaker
            {"BAH" , SaveType::EEPROM_4k}, // EEPROM_V124     Alien Hominid
            {"AEV" , SaveType::NONE}, // NONE}            Alienators - Evolution Continues
            {"BAL" , SaveType::EEPROM_4k}, // EEPROM_V124     All Grown Up - Express Yourself
            {"AA3" , SaveType::SRAM_256K}, // SRAM_V112       All-Star Baseball 2003
            {"AA7" , SaveType::SRAM_256K}, // SRAM_V113       All-Star Baseball 2004
            {"AAR" , SaveType::EEPROM_4k}, // EEPROM_V122     Altered Beast - Guardian of the Realms
            {"AAB" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     American Bass Challenge
            {"BAP" , SaveType::EEPROM_4k}, // EEPROM_V124     American Dragon - Jake Long
            {"BID" , SaveType::NONE}, // NONE}            American Idol
            {"AFG" , SaveType::NONE}, // NONE}            An American Tail - Fievel's Gold Rush
            {"AFN" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Angel Collection - Mezase! Gakuen no Fashion Leader
            {"BEC" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Angel Collection 2 - Pichimo ni Narou
            {"AAG" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Angelique
            {"AAN" , SaveType::SRAM_256K}, // SRAM_V102       Animal Mania - DokiDoki Aishou Check
            {"AAQ" , SaveType::NONE}, // NONE}            Animal Snap - Rescue Them 2 By 2
            {"BAY" , SaveType::SRAM_256K}, // SRAM_V113       Animal Yokochou - Doki Doki Kyushutsu Daisakusen No Maki
            {"BAX" , SaveType::SRAM_256K}, // SRAM_V103       Animal Yokochou - Doki Doki Shinkyuu Shiken! no Kan
            {"ANI" , SaveType::NONE}, // NONE}            Animaniacs - Lights, Camera, Action!
            {"ANU" , SaveType::NONE}, // NONE}            Antz - Extreme Racing
            {"ANZ" , SaveType::NONE}, // NONE}            Antz - Extreme Racing
            {"AAZ" , SaveType::EEPROM_4k}, // EEPROM_V122     Ao-Zoura to Nakamatachi - Yume no Bouken
            {"BPL" , SaveType::NONE}, // NONE}            Archer Maclean's 3D Pool
            {"AZN" , SaveType::NONE}, // NONE}            Archer Maclean's Super Dropzone
            {"BB5" , SaveType::NONE}, // NONE}            Arctic Tale
            {"AME" , SaveType::NONE}, // NONE}            Army Men - Operation Green
            {"AY3" , SaveType::EEPROM_4k}, // EEPROM_V121     Army Men - Turf Wars
            {"ASA" , SaveType::NONE}, // NONE}            Army Men Advance
            {"B8D" , SaveType::NONE}, // NONE}            Around the World in 80 Days
            {"B2N" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Arthur and the Invisibles
            {"BAM" , SaveType::SRAM_256K}, // SRAM_V113       Ashita no Joe - Makka ni Moeagare!
            {"AOB" , SaveType::NONE}, // NONE}            Asterix & Obelix - Bash Them All!
            {"BLX" , SaveType::EEPROM_4k}, // EEPROM_V124     Asterix & Obelix - XXL
            {"BTA" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Astro Boy - Omega Factor
            {"AAV" , SaveType::NONE}, // NONE}            Atari Anniversary Advance
            {"ATL" , SaveType::NONE}, // NONE}            Atlantis - The Lost Empire
            {"BET" , SaveType::NONE}, // NONE}            Atomic Betty
            {"AQR" , SaveType::NONE}, // NONE}            ATV Quad Power Racing
            {"B3B" , SaveType::NONE}, // NONE}            ATV Thunder - Ridge Riders
            {"BQZ" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Avatar - The Last Airbender
            {"BBW" , SaveType::NONE}, // NONE}            Avatar - The Last Airbender - The Burning Earth
            {"AZA" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Azumanga Daiou Advance
            {"BBV" , SaveType::EEPROM_4k}, // EEPROM_V124     Babar - To the Rescue
            {"BBC" , SaveType::NONE}, // NONE}            Back to Stone
            {"ABK" , SaveType::EEPROM_4k}, // EEPROM_V122     BackTrack
            {"ACK" , SaveType::EEPROM_4k}, // EEPROM_V122     Backyard Baseball
            {"BCY" , SaveType::EEPROM_4k}, // EEPROM_V124     Backyard Baseball 2006
            {"AYB" , SaveType::EEPROM_4k}, // EEPROM_V124     Backyard Basketball
            {"AYF" , SaveType::EEPROM_4k}, // EEPROM_V122     Backyard Football
            {"BYH" , SaveType::EEPROM_4k}, // EEPROM_V122     Backyard Hockey
            {"BYF" , SaveType::EEPROM_4k}, // EEPROM_V124     Backyard NFL Football 2006
            {"BS6" , SaveType::EEPROM_4k}, // EEPROM_V124     Backyard Skateboarding
            {"BC7" , SaveType::EEPROM_4k}, // EEPROM_V124     Backyard Sports - Baseball 2007
            {"BB7" , SaveType::EEPROM_4k}, // EEPROM_V124     Backyard Sports - Basketball 2007
            {"BF7" , SaveType::EEPROM_4k}, // EEPROM_V124     Backyard Sports - Football 2007
            {"AHE" , SaveType::SRAM_256K}, // SRAM_V112       Bakuten Shoot Beyblade - Gekitou! Saikyou Blader
            {"AB8" , SaveType::SRAM_256K}, // SRAM_V112       Bakuten Shoot Beyblade 2002 - Ikuze! Bakutou! Chou Jiryoku Battle!!
            {"A3E" , SaveType::SRAM_256K}, // SRAM_V112       Bakuten Shoot Beyblade 2002 - Team Battle!! Daichi Hen
            {"A3W" , SaveType::SRAM_256K}, // SRAM_V112       Bakuten Shoot Beyblade 2002 - Team Battle!! Takao Hen
            {"BGD" , SaveType::EEPROM_4k}, // EEPROM_V124     Baldurs Gate - Dark Alliance
            {"AEE" , SaveType::NONE}, // NONE}            Ballistic - Ecks vs Sever
            {"BAJ" , SaveType::EEPROM_4k}, // EEPROM_V124     Banjo Pilot
            {"BKZ" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Banjo-Kazooie - Grunty's Revenge
            {"BAU" , SaveType::NONE}, // NONE}            Barbie - The Princess and the Pauper
            {"BE5" , SaveType::NONE}, // NONE}            Barbie and the Magic of Pegasus
            {"BBN" , SaveType::EEPROM_4k}, // EEPROM_V124     Barbie as the Island Princess
            {"AVB" , SaveType::NONE}, // NONE}            Barbie Groovy Games
            {"AI8" , SaveType::NONE}, // NONE}            Barbie Horse Adventures - Blue Ribbon Race
            {"BB3" , SaveType::NONE}, // NONE}            Barbie in the 12 Dancing Princesses
            {"BBE" , SaveType::NONE}, // NONE}            Barbie Superpack - Secret Agent + Groovy Games
            {"BBY" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Barnyard
            {"ABP" , SaveType::SRAM_256K}, // SRAM_V102       Baseball Advance
            {"AZB" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Bass Tsuri Shiyouze!
            {"BBG" , SaveType::EEPROM_4k}, // EEPROM_V124     Batman Begins
            {"BAT" , SaveType::NONE}, // NONE}            Batman Rise of Sin Tzu
            {"ABT" , SaveType::NONE}, // NONE}            Batman Vengeance
            {"BDX" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Battle B-Daman
            {"BBM" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Battle B-Daman - Fire Spirits
            {"BBF" , SaveType::EEPROM_4k}, // EEPROM_V122     Battle x Battle - Kyoudai Ou Densetsu
            {"ABE" , SaveType::EEPROM_4k}, // EEPROM_V120     BattleBots - Beyond the BattleBox
            {"BBD" , SaveType::NONE}, // NONE}            BattleBots - Design & Destroy
            {"A8L" , SaveType::SRAM_256K}, // SRAM_V103       BB Ball
            {"AH5" , SaveType::SRAM_256K}, // SRAM_V102       Beast Shooter - Mezase Beast King!
            {"BHB" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Best Friends - Dogs & Cats
            {"A8Y" , SaveType::Flash_512k_SST}, // FLASH_V126      Best Play Pro Yakyuu
            {"BB2" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Beyblade G-Revolution
            {"BEY" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Beyblade VForce - Ultimate Blader Jam
            {"BBX" , SaveType::NONE}, // NONE}            Bibi Blocksberg - Der Magische Hexenkreis
            {"BUX" , SaveType::EEPROM_4k}, // EEPROM_V124     Bibi und Tina - Ferien auf dem Martinshof
            {"BIB" , SaveType::NONE}, // NONE}            Bible Game, The
            {"B63" , SaveType::EEPROM_4k}, // EEPROM_V124     Big Mutha Truckers
            {"BIO" , SaveType::NONE}, // NONE}            Bionicle
            {"A5A" , SaveType::EEPROM_4k}, // EEPROM_V120     Bionicle - Matoran Adventures
            {"BIL" , SaveType::EEPROM_4k}, // EEPROM_V124     Bionicle - Maze of Shadows
            {"BIH" , SaveType::EEPROM_4k}, // EEPROM_V124     Bionicle Heroes
            {"BVD" , SaveType::SRAM_256K}, // SRAM_V113       bit Generations - Boundish
            {"BVA" , SaveType::SRAM_256K}, // SRAM_V113       bit Generations - Coloris
            {"BVB" , SaveType::SRAM_256K}, // SRAM_V113       bit Generations - Dial Hex
            {"BVH" , SaveType::SRAM_256K}, // SRAM_V103       bit Generations - Digidrive
            {"BVC" , SaveType::SRAM_256K}, // SRAM_V113       bit Generations - Dotstream
            {"BVE" , SaveType::SRAM_256K}, // SRAM_V113       bit Generations - Orbital
            {"BVG" , SaveType::SRAM_256K}, // SRAM_V113       bit Generations - Soundvoyager
            {"AB6" , SaveType::EEPROM_4k}, // EEPROM_V120     Black Belt Challenge
            {"AWE" , SaveType::SRAM_256K}, // SRAM_V112       Black Black - Bura Bura
            {"AXB" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Black Matrix Zero
            {"AQX" , SaveType::EEPROM_4k}, // EEPROM_V122     Blackthorne
            {"BBH" , SaveType::NONE}, // NONE}            Blades of Thunder
            {"BLE" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Bleach Advance
            {"ABR" , SaveType::SRAM_256K}, // SRAM_V112       Blender Bros.
            {"AT9" , SaveType::EEPROM_4k}, // EEPROM_V122     BMX Trick Racer
            {"B6E" , SaveType::NONE}, // NONE}            Board Games Classics
            {"BB9" , SaveType::SRAM_256K}, // SRAM_V113       Boboboubo Boubobo - 9 Kiwame Senshi Gyagu Yuugou
            {"BOB" , SaveType::SRAM_256K}, // SRAM_V103       Boboboubo Boubobo - Maji De!! Shinken Battle
            {"A8V" , SaveType::SRAM_256K}, // SRAM_V112       Boboboubo Boubobo - Ougi 87.5 Bakuretsu Hanage Shinken
            {"BOS" , SaveType::SRAM_256K}, // SRAM_V113       Boboboubo Boubobo Bakutou Hajike Taisen
            {"U3I" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Boktai - The Sun is in Your Hand
            {"U32" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Boktai 2 - Solar Boy Django
            {"ABC" , SaveType::SRAM_256K}, // SRAM_V110       Boku ha Koukuu Kanseikan
            {"AJZ" , SaveType::SRAM_256K}, // SRAM_V112       Bomberman Jetters
            {"BOM" , SaveType::EEPROM_4k}, // EEPROM_V122     Bomberman Jetters - Game Collection
            {"AMH" , SaveType::EEPROM_4k_alt}, // EEPROM_V121     Bomberman Max 2 - Blue Advance
            {"AMY" , SaveType::EEPROM_4k_alt}, // EEPROM_V121     Bomberman Max 2 - Red Advance
            {"ABS" , SaveType::SRAM_256K}, // SRAM_V112       Bomberman Tournament
            {"BKW" , SaveType::EEPROM_4k}, // EEPROM_V122     Bookworm
            {"BPD" , SaveType::SRAM_256K}, // SRAM_V113       Bouken Yuuki PlaStar World - Densetsu no PlaStar Gate EX
            {"APJ" , SaveType::SRAM_256K}, // SRAM_V113       Bouken Yuuki Pluster World - Densetsu no PlustoGate
            {"A2P" , SaveType::SRAM_256K}, // SRAM_V112       Bouken Yuuki Pluster World - Pluston GP
            {"BOV" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Bouken-Ou Beet - Busters Road
            {"BBS" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Boukyaku no Senritsu
            {"ABD" , SaveType::EEPROM_4k}, // EEPROM_V122     Boulder Dash EX
            {"ABO" , SaveType::NONE}, // NONE}            Boxing Fever
            {"A2R" , SaveType::EEPROM_4k}, // EEPROM_V122     Bratz
            {"BBZ" , SaveType::NONE}, // NONE}            Bratz - Babyz
            {"BXF" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Bratz - Forever Diamondz
            {"BRR" , SaveType::EEPROM_4k}, // EEPROM_V124     Bratz - Rock Angelz
            {"BBU" , SaveType::NONE}, // NONE}            Bratz - The Movie
            {"B6Z" , SaveType::NONE}, // NONE}            Breakout + Centipede + Warlords
            {"ABF" , SaveType::SRAM_256K}, // SRAM_V112       Breath of Fire
            {"AB2" , SaveType::SRAM_256K}, // SRAM_V112       Breath of Fire II
            {"ABY" , SaveType::NONE}, // NONE}            Britney's Dance Beat
            {"ABJ" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Broken Sword - The Shadow of the Templars
            {"BBR" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Brother Bear
            {"ALE" , SaveType::EEPROM_4k}, // EEPROM_V122     Bruce Lee - Return of the Legend
            {"AON" , SaveType::EEPROM_4k}, // EEPROM_V122     Bubble Bobble - Old & New
            {"A2B" , SaveType::EEPROM_4k}, // EEPROM_V122     Bubble Bobble - Old & New
            {"AVY" , SaveType::EEPROM_4k}, // EEPROM_V122     Buffy - Im Bann der Daemonen
            {"ABW" , SaveType::NONE}, // NONE}            Butt-Ugly Martians - B.K.M. Battles
            {"AUQ" , SaveType::NONE}, // NONE}            Butt-Ugly Martians - B.K.M. Battles
            {"BCG" , SaveType::EEPROM_4k}, // EEPROM_V124     Cabbage Patch Kids - The Patch Puppy Rescue
            {"A8H" , SaveType::NONE}, // NONE}            Cabela's Big Game Hunter
            {"BG5" , SaveType::EEPROM_4k}, // EEPROM_V124     Cabela's Big Game Hunter - 2005 Adventures
            {"ACP" , SaveType::NONE}, // NONE}            Caesars Palace Advance
            {"BIX" , SaveType::Flash_1M_Macronix}, // FLASH_V102      Calciobit
            {"BLC" , SaveType::EEPROM_4k}, // EEPROM_V124     Camp Lazlo - Leaky Lake Games
            {"BC6" , SaveType::NONE}, // NONE}            Capcom Classics - Mini Mix
            {"AKY" , SaveType::SRAM_256K}, // SRAM_V112       Captain Tsubasa - Eikou no Kiseki
            {"ACB" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Car Battler Joe
            {"BK3" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Card Captor Sakura - Sakura Card de Mini Game
            {"BKS" , SaveType::EEPROM_4k}, // EEPROM_V124     Card Captor Sakura - Sakura to Card to Otomodachi
            {"PEA" , SaveType::Flash_512k_SST}, // FLASH_V124      Card E-Reader
            {"A8C" , SaveType::NONE}, // NONE}            Card Party
            {"BEA" , SaveType::EEPROM_4k}, // EEPROM_V124     Care Bears - The Care Quest
            {"AED" , SaveType::EEPROM_4k}, // EEPROM_V122     Carrera Power Slide
            {"BCA" , SaveType::EEPROM_4k}, // EEPROM_V124     Cars
            {"BCP" , SaveType::NONE}, // NONE}            Cars Mater-National Championship
            {"AC9" , SaveType::NONE}, // NONE}            Cartoon Network Block Party
            {"ANR" , SaveType::NONE}, // NONE}            Cartoon Network Speedway
            {"ACS" , SaveType::NONE}, // NONE}            Casper
            {"A2C" , SaveType::SRAM_256K}, // SRAM_V102       Castlevania - Aria of Sorrow
            {"AAM" , SaveType::SRAM_256K}, // SRAM_V110       Castlevania - Circle of the Moon
            {"ACH" , SaveType::SRAM_256K}, // SRAM_V102       Castlevania - Harmony of Dissonance
            {"BXK" , SaveType::SRAM_256K}, // SRAM_V102       Castlevania Double Pack
            {"BCW" , SaveType::EEPROM_4k}, // EEPROM_V120     Catwoman
            {"AN3" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Catz
            {"BCF" , SaveType::EEPROM_4k}, // EEPROM_V124     Charlie and the Chocolate Factory
            {"BCJ" , SaveType::EEPROM_4k}, // EEPROM_V124     Charlotte's Web
            {"ACY" , SaveType::EEPROM_4k}, // EEPROM_V122     Chessmaster
            {"BCH" , SaveType::EEPROM_4k}, // EEPROM_V124     Chicken Little
            {"B6F" , SaveType::NONE}, // NONE}            Chicken Shoot
            {"B6G" , SaveType::NONE}, // NONE}            Chicken Shoot 2
            {"AOC" , SaveType::EEPROM_4k}, // EEPROM_V122     Chobits for GameBoy Advance
            {"A5B" , SaveType::SRAM_256K}, // SRAM_V112       Chocobo Land - A Game of Dice
            {"ACJ" , SaveType::EEPROM_4k}, // EEPROM_V122     Chou Makai-Mura R
            {"ACR" , SaveType::Flash_512k_Atmel}, // FLASH_V121      Chu Chu Rocket!
            {"BCM" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     CIMA - The Enemy
            {"BCD" , SaveType::EEPROM_4k}, // EEPROM_V124     Cinderella - Magical Dreams
            {"B2S" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Cinnamon - Yume no Daibouken
            {"B43" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Cinnamon Fuwafuwa Daisakusen
            {"BPS" , SaveType::EEPROM_4k}, // EEPROM_V124     Cinnamoroll Kokoniiruyo
            {"FBM" , SaveType::EEPROM_4k}, // EEPROM_V124     Classic NES Series - Bomberman
            {"FAD" , SaveType::EEPROM_4k}, // EEPROM_V124     Classic NES Series - Castlevania
            {"FDK" , SaveType::EEPROM_4k}, // EEPROM_V122     Classic NES Series - Donkey Kong
            {"FDM" , SaveType::EEPROM_4k}, // EEPROM_V124     Classic NES Series - Dr. Mario
            {"FEB" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Classic NES Series - ExciteBike
            {"FIC" , SaveType::EEPROM_4k}, // EEPROM_V122     Classic NES Series - Ice Climber
            {"FMR" , SaveType::EEPROM_4k}, // EEPROM_V124     Classic NES Series - Metroid
            {"FP7" , SaveType::EEPROM_4k}, // EEPROM_V122     Classic NES Series - Pac-Man
            {"FSM" , SaveType::EEPROM_4k}, // EEPROM_V124     Classic NES Series - Super Mario Bros.
            {"FZL" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Classic NES Series - The Legend of Zelda
            {"FXV" , SaveType::EEPROM_4k}, // EEPROM_V122     Classic NES Series - Xevious
            {"FLB" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Classic NES Series - Zelda II - The Adventure of Link
            {"BC5" , SaveType::NONE}, // NONE}            Cocoto - Kart Racer
            {"BC8" , SaveType::NONE}, // NONE}            Cocoto Platform Jumper
            {"BND" , SaveType::NONE}, // NONE}            Codename Kids Next Door - Operation S.O.D.A.
            {"ACM" , SaveType::EEPROM_4k}, // EEPROM_V122     Colin McRae Rally 2.0
            {"ACG" , SaveType::EEPROM_4k}, // EEPROM_V122     Columns Crown
            {"AQC" , SaveType::SRAM_256K}, // SRAM_V112       Combat Choro Q - Advance Daisakusen
            {"BW5" , SaveType::Flash_512k_Panasonic}, // FLASH_V133      Combo Pack - Sonic Advance + Sonic Pinball Party
            {"ACZ" , SaveType::EEPROM_4k}, // EEPROM_V122     Comix Zone
            {"B65" , SaveType::NONE}, // NONE}            Connect Four - Perfection - Trouble
            {"AAW" , SaveType::NONE}, // NONE}            Contra Advance - The Alien Wars EX
            {"AVC" , SaveType::EEPROM_4k}, // EEPROM_V122     Corvette
            {"B5A" , SaveType::EEPROM_4k}, // EEPROM_V122     Crash & Spyro - Super Pack Volume 1
            {"B52" , SaveType::EEPROM_4k}, // EEPROM_V122     Crash & Spyro - Super Pack Volume 2
            {"B53" , SaveType::EEPROM_4k_alt}, // EEPROM_V126     Crash & Spyro Superpack - Ripto's Rampage + The Cortex Conspiracy
            {"B54" , SaveType::EEPROM_4k}, // EEPROM_V122     Crash & Spyro Superpack - The Huge Adventure + Season of Ice
            {"ACQ" , SaveType::EEPROM_4k}, // EEPROM_V122     Crash Bandicoot - The Huge Adventure
            {"AC8" , SaveType::EEPROM_4k}, // EEPROM_V122     Crash Bandicoot 2 - N-Tranced
            {"ACU" , SaveType::EEPROM_4k}, // EEPROM_V122     Crash Bandicoot Advance
            {"BKD" , SaveType::EEPROM_4k}, // EEPROM_V124     Crash Bandicoot Advance - Wakuwaku Tomodachi Daisakusen
            {"BD4" , SaveType::EEPROM_4k}, // EEPROM_V124     Crash Bandicoot Purple - Ripto's Rampage
            {"BCN" , SaveType::EEPROM_4k}, // EEPROM_V122     Crash Nitro Kart
            {"BQC" , SaveType::EEPROM_4k}, // EEPROM_V124     Crash of the Titans
            {"B8A" , SaveType::EEPROM_8k}, // EEPROM_V122     Crash Superpack - N-Tranced + Nitro Kart
            {"BC2" , SaveType::EEPROM_8k}, // EEPROM_V126     Crayon Shin chan - Densetsu wo Yobu Omake no Miyako Shockgaan
            {"BKC" , SaveType::EEPROM_4k}, // EEPROM_V124     Crayon Shin-Chan - Arashi no Yobu Cinema-Land no Daibouken!
            {"ACC" , SaveType::EEPROM_4k}, // EEPROM_V122     Crazy Chase
            {"BCR" , SaveType::EEPROM_4k}, // EEPROM_V124     Crazy Frog Racer
            {"A3C" , SaveType::EEPROM_4k}, // EEPROM_V122     Crazy Taxi - Catch a Ride
            {"ACT" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Creatures
            {"A6C" , SaveType::SRAM_256K}, // SRAM_V102       Croket! - Yume no Banker Survival!
            {"BK2" , SaveType::SRAM_256K}, // SRAM_V103       Croket! 2 - Yami no Bank to Banqueen
            {"B3K" , SaveType::SRAM_256K}, // SRAM_V103       Croket! 3 - Guranyuu Oukoku no Nazo
            {"BK4" , SaveType::SRAM_256K}, // SRAM_V103       Croket! 4 - Bank no Mori no Mamorigami
            {"AQD" , SaveType::NONE}, // NONE}            Crouching Tiger Hidden Dragon
            {"ACF" , SaveType::NONE}, // NONE}            Cruis'n Velocity
            {"BCB" , SaveType::EEPROM_4k}, // EEPROM_V124     Crushed Baseball
            {"AC7" , SaveType::NONE}, // NONE}            CT Special Forces
            {"A9C" , SaveType::NONE}, // NONE}            CT Special Forces 2 - Back in the Trenches
            {"BC3" , SaveType::NONE}, // NONE}            CT Special Forces 3 - Bioterror
            {"ACX" , SaveType::EEPROM_4k}, // EEPROM_V120     Cubix - Robots for Everyone - Clash 'N Bash
            {"B3J" , SaveType::NONE}, // NONE}            Curious George
            {"ARJ" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Custom Robo GX
            {"AZ3" , SaveType::SRAM_256K}, // SRAM_V112       Cyberdrive Zoids - Kijuu no Senshi Hyuu
            {"AHM" , SaveType::EEPROM_4k}, // EEPROM_V122     Dai-Mahjong
            {"ADS" , SaveType::Flash_512k_SST}, // FLASH_V126      Daisenryaku for GameBoy Advance
            {"ATD" , SaveType::SRAM_256K}, // SRAM_V102       Daisuki Teddy
            {"BDN" , SaveType::SRAM_256K}, // SRAM_V113       Dan Doh!! - Tobase Shouri no Smile Shot!!
            {"AXH" , SaveType::SRAM_256K}, // SRAM_V112       Dan Doh!! Xi
            {"A9S" , SaveType::EEPROM_4k}, // EEPROM_V122     Dancing Sword - Senkou
            {"BUE" , SaveType::EEPROM_4k}, // EEPROM_V124     Danny Phantom - The Ultimate Enemy
            {"BQY" , SaveType::EEPROM_4k}, // EEPROM_V124     Danny Phantom - Urban Jungle
            {"AVL" , SaveType::NONE}, // NONE}            Daredevil
            {"A2D" , SaveType::EEPROM_4k}, // EEPROM_V122     Darius R
            {"ADA" , SaveType::NONE}, // NONE}            Dark Arena
            {"AX2" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Dave Mirra Freestyle BMX 2
            {"AB3" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Dave Mirra Freestyle BMX 3
            {"ABQ" , SaveType::EEPROM_4k}, // EEPROM_V122     David Beckham Soccer
            {"AD6" , SaveType::EEPROM_4k}, // EEPROM_V122     Davis Cup
            {"BDE" , SaveType::NONE}, // NONE}            Dead to Rights
            {"BZN" , SaveType::NONE}, // NONE}            Deal or No Deal
            {"A2F" , SaveType::EEPROM_4k}, // EEPROM_V122     Defender
            {"ADH" , SaveType::EEPROM_4k}, // EEPROM_V122     Defender of the Crown
            {"AC5" , SaveType::SRAM_256K}, // SRAM_V103       DemiKids - Dark Version
            {"AL4" , SaveType::SRAM_256K}, // SRAM_V103       DemiKids - Light Version
            {"A9A" , SaveType::NONE}, // NONE}            Demon Driver - Time to Burn Rubber!
            {"ADB" , SaveType::EEPROM_4k}, // EEPROM_V122     Denki Blocks!
            {"AST" , SaveType::SRAM_256K}, // SRAM_V112       Densetsu no Stafi
            {"AVF" , SaveType::SRAM_256K}, // SRAM_V113       Densetsu no Stafi 2
            {"B3D" , SaveType::SRAM_256K}, // SRAM_V113       Densetsu no Stafi 3
            {"A8P" , SaveType::Flash_512k_SST}, // FLASH_V126      Derby Stallion Advance
            {"ADI" , SaveType::NONE}, // NONE}            Desert Strike Advance
            {"ADX" , SaveType::NONE}, // NONE}            Dexter's Laboratory - Chess Challenge
            {"ADL" , SaveType::EEPROM_4k}, // EEPROM_V122     Dexter's Laboratory - Deesaster Strikes!
            {"A3O" , SaveType::SRAM_256K}, // SRAM_V102       Di Gi Charat - DigiCommunication
            {"ADD" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Diadroids World - Evil Teikoku no Yabou
            {"BXW" , SaveType::EEPROM_4k}, // EEPROM_V124     Die Wilden Fussball Kerle - Gefahr im Wilde Kerle Land
            {"BWU" , SaveType::NONE}, // NONE}            Die wilden Fussballkerle - Entscheidung im Teufelstopf
            {"BDK" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Digi Communication 2 in 1 Datou! Black Gemagema Dan
            {"A8S" , SaveType::EEPROM_4k}, // EEPROM_V122     Digimon - Battle Spirit
            {"BDS" , SaveType::EEPROM_4k}, // EEPROM_V122     Digimon - Battle Spirit 2
            {"BDG" , SaveType::EEPROM_4k}, // EEPROM_V124     Digimon Racing
            {"BDJ" , SaveType::EEPROM_4k}, // EEPROM_V124     Digimon Racing
            {"AD3" , SaveType::EEPROM_4k}, // EEPROM_V122     Dinotopia - The Timestone Pirates
            {"AQP" , SaveType::NONE}, // NONE}            Disney Princess
            {"BQN" , SaveType::EEPROM_4k}, // EEPROM_V124     Disney Princess - Royal Adventure
            {"A2A" , SaveType::EEPROM_4k}, // EEPROM_V122     Disney Sports - Basketball
            {"A3D" , SaveType::EEPROM_4k}, // EEPROM_V122     Disney Sports - Football
            {"AOM" , SaveType::EEPROM_4k}, // EEPROM_V122     Disney Sports - Motocross
            {"A4D" , SaveType::EEPROM_4k}, // EEPROM_V122     Disney Sports - Skateboarding
            {"A5D" , SaveType::EEPROM_4k}, // EEPROM_V122     Disney Sports - Snowboarding
            {"A6D" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Disney Sports - Soccer
            {"BD8" , SaveType::EEPROM_4k}, // EEPROM_V122     Disney's Party
            {"BBK" , SaveType::SRAM_256K}, // SRAM_V113       DK - King of Swing
            {"B82" , SaveType::EEPROM_4k}, // EEPROM_V124     Dogz
            {"BFE" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Dogz - Fashion
            {"BIM" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Dogz 2
            {"ADQ" , SaveType::SRAM_256K}, // SRAM_V102       Dokapon
            {"A56" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     DokiDoki Cooking Series 1 - Komugi-chan no Happy Cake
            {"A8O" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     DokiDoki Cooking Series 2 - Gourmet Kitchen - Suteki na Obentou
            {"AYA" , SaveType::SRAM_256K}, // SRAM_V102       Dokodemo Taikyoku - Yakuman Advance
            {"ADO" , SaveType::SRAM_256K}, // SRAM_V112       Domo-kun no Fushigi Terebi
            {"ADK" , SaveType::NONE}, // NONE}            Donald Duck Advance
            {"AAD" , SaveType::NONE}, // NONE}            Donald Duck Advance
            {"BDA" , SaveType::EEPROM_4k}, // EEPROM_V124     Donchan Puzzle Hanabi de Dohn Advance
            {"A5N" , SaveType::EEPROM_4k}, // EEPROM_V122     Donkey Kong Country
            {"B2D" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Donkey Kong Country 2
            {"BDQ" , SaveType::EEPROM_4k}, // EEPROM_V124     Donkey Kong Country 3
            {"ADM" , SaveType::EEPROM_4k}, // EEPROM_V120     Doom
            {"A9D" , SaveType::EEPROM_4k}, // EEPROM_V120     Doom II
            {"BXP" , SaveType::NONE}, // NONE}            Dora the Explorer - Dora's World Adventure
            {"BER" , SaveType::NONE}, // NONE}            Dora the Explorer - Super Spies
            {"BDO" , SaveType::NONE}, // NONE}            Dora the Explorer - Super Star Adventures!
            {"AER" , SaveType::NONE}, // NONE}            Dora the Explorer - The Search for the Pirate Pig's Treasure
            {"ADP" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Doraemon - Dokodemo Walker
            {"ADR" , SaveType::SRAM_256K}, // SRAM_V110       Doraemon - Midori no Wakusei DokiDoki Daikyuushutsu!
            {"BDD" , SaveType::NONE}, // NONE}            Double Dragon Advance
            {"A8D" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Doubutsu Shima no Chobi Gurumi
            {"BDC" , SaveType::EEPROM_4k}, // EEPROM_V124     Doubutsujima no Chobi Gurumi 2 - Tamachan Monogatari
            {"ADW" , SaveType::NONE}, // NONE}            Downforce
            {"A6T" , SaveType::EEPROM_4k}, // EEPROM_V121     Dr. Muto
            {"AQT" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Dr. Seuss' - The Cat in the Hat
            {"BUO" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Dr. Sudoku
            {"BDV" , SaveType::EEPROM_4k}, // EEPROM_V124     Dragon Ball - Advanced Adventure
            {"BT4" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Dragon Ball GT - Transformation
            {"BG3" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Dragon Ball Z - Buu's Fury
            {"ADZ" , SaveType::SRAM_256K}, // SRAM_V102       Dragon Ball Z - Collectible Card Game
            {"AZJ" , SaveType::EEPROM_4k}, // EEPROM_V124     Dragon Ball Z - Supersonic Warriors
            {"BDB" , SaveType::EEPROM_4k}, // EEPROM_V122     Dragon Ball Z - Taiketsu
            {"ALG" , SaveType::EEPROM_4k}, // EEPROM_V122     Dragon Ball Z - The Legacy of Goku
            {"ALF" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Dragon Ball Z - The Legacy of Goku II
            {"A5G" , SaveType::SRAM_256K}, // SRAM_V113       Dragon Drive - World D Break
            {"AT2" , SaveType::Flash_512k_Atmel}, // FLASH_V121      Dragon Quest Characters - Torneko no Daibouken 2 Advance
            {"BD3" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Dragon Quest Characters - Torneko no Daibouken 3 Advance
            {"A9H" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Dragon Quest Monsters - Caravan Heart
            {"BD9" , SaveType::NONE}, // NONE}            Dragon Tales - Dragon Adventures
            {"BJD" , SaveType::EEPROM_4k}, // EEPROM_V124     Dragon's Rock
            {"AJY" , SaveType::NONE}, // NONE}            Drake & Josh
            {"V49" , SaveType::SRAM_256K}, // SRAM_V113       Drill Dozer
            {"B3R" , SaveType::EEPROM_4k}, // EEPROM_V124     Driv3r
            {"ADV" , SaveType::EEPROM_4k}, // EEPROM_V120     Driven
            {"ADU" , SaveType::EEPROM_4k}, // EEPROM_V122     Driver 2 Advance
            {"AOE" , SaveType::EEPROM_4k}, // EEPROM_V122     Drome Racers
            {"AD7" , SaveType::NONE}, // NONE}            Droopy's Tennis Open
            {"AB9" , SaveType::EEPROM_4k}, // EEPROM_V122     Dual Blades
            {"BD6" , SaveType::EEPROM_4k}, // EEPROM_V124     Duel Masters - Kaijudo Showdown
            {"AA9" , SaveType::EEPROM_4k}, // EEPROM_V124     Duel Masters - Sempai Legends
            {"BDU" , SaveType::EEPROM_4k}, // EEPROM_V124     Duel Masters - Shadow of the Code
            {"BD2" , SaveType::SRAM_256K}, // SRAM_V113       Duel Masters 2
            {"BD5" , SaveType::SRAM_256K}, // SRAM_V113       Duel Masters 2 - Kirifuda Shoubu Version
            {"AD9" , SaveType::EEPROM_4k}, // EEPROM_V120     Duke Nukem Advance
            {"AD4" , SaveType::SRAM_256K}, // SRAM_V102       Dungeons & Dragons - Eye of the Beholder
            {"B36" , SaveType::SRAM_256K}, // SRAM_V113       Dynasty Warriors Advance
            {"AET" , SaveType::NONE}, // NONE}            E.T. - The Extra-Terrestrial
            {"AEJ" , SaveType::NONE}, // NONE}            Earthworm Jim
            {"AJ4" , SaveType::NONE}, // NONE}            Earthworm Jim 2
            {"AES" , SaveType::NONE}, // NONE}            Ecks vs Sever
            {"AE3" , SaveType::EEPROM_4k}, // EEPROM_V122     Ed, Edd n Eddy - Jawbreakers!
            {"BED" , SaveType::EEPROM_4k}, // EEPROM_V124     Ed, Edd n Eddy - The Mis-Edventures
            {"AEM" , SaveType::NONE}, // NONE}            Egg Mania
            {"AEK" , SaveType::EEPROM_4k}, // EEPROM_V122     Elemix!
            {"ANW" , SaveType::EEPROM_4k}, // EEPROM_V122     Elevator Action - Old & New
            {"BEL" , SaveType::NONE}, // NONE}            Elf - The Movie
            {"BEB" , SaveType::NONE}, // NONE}            Elf Bowling 1 & 2
            {"BZR" , SaveType::EEPROM_4k}, // EEPROM_V124     Enchanted - Once Upon Andalasia
            {"BEN" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Eragon
            {"PSA" , SaveType::Flash_1M_Macronix_RTC}, // FLASH_V103      E-Reader
            {"BEJ" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Erementar Gerad
            {"AGR" , SaveType::Flash_512k_SST}, // FLASH_V124      ESPN Final Round Golf 2002
            {"AMG" , SaveType::SRAM_256K}, // SRAM_V112       ESPN Great Outdoor Games - Bass 2002
            {"AWI" , SaveType::EEPROM_4k}, // EEPROM_V122     ESPN International Winter Sports 2002
            {"AWX" , SaveType::EEPROM_4k}, // EEPROM_V122     ESPN Winter X-Games Snowboarding 2002
            {"AXS" , SaveType::EEPROM_4k}, // EEPROM_V121     ESPN X-Games Skateboarding
            {"AEL" , SaveType::NONE}, // NONE}            European Super League
            {"BEV" , SaveType::EEPROM_4k}, // EEPROM_V124     Ever Girl
            {"AMO" , SaveType::SRAM_256K}, // SRAM_V112       EX Monopoly
            {"AEG" , SaveType::NONE}, // NONE}            Extreme Ghostbusters - Code Ecto-1
            {"BES" , SaveType::EEPROM_4k}, // EEPROM_V122     Extreme Skate Adventure
            {"BE4" , SaveType::SRAM_256K}, // SRAM_V113       Eyeshield 21 - Devilbats Devildays
            {"A22" , SaveType::SRAM_256K}, // SRAM_V110       EZ-Talk - Shokyuu Hen 1
            {"A23" , SaveType::SRAM_256K}, // SRAM_V110       EZ-Talk - Shokyuu Hen 2
            {"A24" , SaveType::SRAM_256K}, // SRAM_V110       EZ-Talk - Shokyuu Hen 3
            {"A25" , SaveType::SRAM_256K}, // SRAM_V110       EZ-Talk - Shokyuu Hen 4
            {"A26" , SaveType::SRAM_256K}, // SRAM_V110       EZ-Talk - Shokyuu Hen 5
            {"A27" , SaveType::SRAM_256K}, // SRAM_V110       EZ-Talk - Shokyuu Hen 6
            {"AF8" , SaveType::EEPROM_4k}, // EEPROM_V122     F1 2002
            {"AFT" , SaveType::NONE}, // NONE}            F-14 Tomcat
            {"BYA" , SaveType::NONE}, // NONE}            F24 - Stealth Fighter
            {"FSR" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Famicom Mini Series - Dai 2 Ji Super Robot Taisen
            {"FGZ" , SaveType::EEPROM_4k}, // EEPROM_V124     Famicom Mini Series - Kido Senshi Z Gundam Hot Scramble
            {"FSO" , SaveType::EEPROM_4k}, // EEPROM_V122     Famicom Mini Series 10 - Star Soldier
            {"FMB" , SaveType::EEPROM_4k}, // EEPROM_V124     Famicom Mini Series 11 - Mario Bros.
            {"FCL" , SaveType::EEPROM_4k}, // EEPROM_V124     Famicom Mini Series 12 - Clu Clu Land
            {"FBF" , SaveType::EEPROM_4k}, // EEPROM_V124     Famicom Mini Series 13 - Balloon Fight
            {"FWC" , SaveType::EEPROM_4k}, // EEPROM_V124     Famicom Mini Series 14 - Wrecking Crew
            {"FDD" , SaveType::EEPROM_4k}, // EEPROM_V124     Famicom Mini Series 16 - Dig Dug
            {"FTB" , SaveType::EEPROM_4k}, // EEPROM_V124     Famicom Mini Series 17 - Takahashi Meijin no Bouken Jima
            {"FMK" , SaveType::EEPROM_4k}, // EEPROM_V124     Famicom Mini Series 18 - Makaimura
            {"FTW" , SaveType::EEPROM_4k}, // EEPROM_V124     Famicom Mini Series 19 - TwinBee
            {"FGG" , SaveType::EEPROM_4k}, // EEPROM_V124     Famicom Mini Series 20 - Ganbare Goemon! Karakuri Douchuu
            {"FM2" , SaveType::EEPROM_4k}, // EEPROM_V124     Famicom Mini Series 21 - Super Mario Bros. 2
            {"FNM" , SaveType::EEPROM_4k}, // EEPROM_V124     Famicom Mini Series 22 - Nazo no Murasame Shiro
            {"FPT" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Famicom Mini Series 24 - Hikari Shinwa - Palutena no Kagame
            {"FFM" , SaveType::EEPROM_4k}, // EEPROM_V124     Famicom Mini Series 26 - Mukashi Hanashi - Shin Onigashima
            {"FTK" , SaveType::EEPROM_4k}, // EEPROM_V124     Famicom Mini Series 27 - Famicom Tantei Club - Kieta Koukeisha
            {"FTU" , SaveType::EEPROM_4k}, // EEPROM_V124     Famicom Mini Series 28 - Famicom Tantei Club Part II - Ushiro ni Tatsu Shoujo
            {"FSD" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Famicom Mini Series 30 - SD Gundam World - Gachapon Senshi Scramble Wars
            {"FPM" , SaveType::EEPROM_4k}, // EEPROM_V124     Famicom Mini Series 6 - Pac-Man
            {"FMP" , SaveType::EEPROM_4k}, // EEPROM_V124     Famicom Mini Series 8 - Mappy
            {"B2F" , SaveType::NONE}, // NONE}            Family Feud
            {"AAT" , SaveType::EEPROM_4k}, // EEPROM_V122     Family Tennis Advance
            {"AN7" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Famista Advance
            {"AJE" , SaveType::SRAM_256K}, // SRAM_V112       Fancy Pocket
            {"BFC" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Fantasic Children
            {"BF4" , SaveType::EEPROM_4k}, // EEPROM_V124     Fantastic 4
            {"BH4" , SaveType::EEPROM_4k}, // EEPROM_V124     Fantastic 4 - Flame On
            {"AAX" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Fantastic Maerchen - Cake-yasan Monogatari
            {"BFU" , SaveType::EEPROM_4k}, // EEPROM_V124     Fear Factor Unleashed
            {"AF9" , SaveType::SRAM_256K}, // SRAM_V112       Field of Nine - Digital Edition 2001
            {"BF6" , SaveType::EEPROM_4k}, // EEPROM_V124     FIFA 06
            {"B7F" , SaveType::EEPROM_4k}, // EEPROM_V124     FIFA 07
            {"AFJ" , SaveType::EEPROM_4k}, // EEPROM_V122     FIFA 2003
            {"BFI" , SaveType::EEPROM_4k}, // EEPROM_V122     FIFA 2004
            {"BF5" , SaveType::EEPROM_4k}, // EEPROM_V124     FIFA 2005
            {"B6W" , SaveType::EEPROM_4k}, // EEPROM_V124     FIFA World Cup 2006
            {"BOX" , SaveType::EEPROM_4k}, // EEPROM_V122     FightBox
            {"AFL" , SaveType::EEPROM_4k}, // EEPROM_V122     FILA Decathlon
            {"BFF" , SaveType::SRAM_256K}, // SRAM_V113       Final Fantasy I & II - Dawn of Souls
            {"BZ4" , SaveType::SRAM_256K}, // SRAM_V113       Final Fantasy IV Advance
            {"AFX" , SaveType::Flash_512k_Panasonic}, // FLASH_V130      Final Fantasy Tactics Advance
            {"BZ5" , SaveType::SRAM_256K}, // SRAM_V113       Final Fantasy V Advance
            {"BZ6" , SaveType::SRAM_256K}, // SRAM_V113       Final Fantasy VI Advance
            {"AFF" , SaveType::EEPROM_4k}, // EEPROM_V121     Final Fight One
            {"AFW" , SaveType::Flash_512k_SST}, // FLASH_V126      Final Fire Pro Wrestling - Yume no Dantai Unei!
            {"AZI" , SaveType::NONE}, // NONE}            Finding Nemo
            {"BFN" , SaveType::NONE}, // NONE}            Finding Nemo
            {"BZI" , SaveType::NONE}, // NONE}            Finding Nemo - The Continuing Adventures
            {"AE7" , SaveType::SRAM_256K}, // SRAM_V102       Fire Emblem
            {"AFE" , SaveType::SRAM_256K}, // SRAM_V102       Fire Emblem - Fuuin no Tsurugi
            {"BE8" , SaveType::SRAM_256K}, // SRAM_V103       Fire Emblem - The Sacred Stones
            {"AFP" , SaveType::Flash_512k_Atmel}, // FLASH_V123      Fire Pro Wrestling
            {"AFY" , SaveType::Flash_512k_SST}, // FLASH_V126      Fire Pro Wrestling 2
            {"BLH" , SaveType::NONE}, // NONE}            Flushed Away
            {"BF3" , SaveType::NONE}, // NONE}            Ford Racing 3
            {"AFM" , SaveType::SRAM_256K}, // SRAM_V112       Formation Soccer 2002
            {"AFO" , SaveType::NONE}, // NONE}            Fortress
            {"BFY" , SaveType::EEPROM_4k}, // EEPROM_V124     Foster's Home for Imaginary Friends
            {"BFK" , SaveType::EEPROM_4k}, // EEPROM_V124     Franklin the Turtle
            {"BFL" , SaveType::EEPROM_4k}, // EEPROM_V124     Franklin's Great Adventures
            {"BFS" , SaveType::NONE}, // NONE}            Freekstyle
            {"AFQ" , SaveType::NONE}, // NONE}            Frogger Advance - The Great Quest
            {"AFR" , SaveType::EEPROM_4k}, // EEPROM_V121     Frogger's Adventures - Temple of the Frog
            {"AFB" , SaveType::EEPROM_4k}, // EEPROM_V122     Frogger's Adventures 2 - The Lost Wand
            {"BFJ" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Frogger's Journey - The Forgotten Relic
            {"ADC" , SaveType::NONE}, // NONE}            Fruit Chase
            {"BFD" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Fruit Mura no Doubutsu Tachi
            {"AF4" , SaveType::EEPROM_4k}, // EEPROM_V122     Fushigi no Kuni no Alice
            {"AFA" , SaveType::SRAM_256K}, // SRAM_V110       Fushigi no Kuni no Angelique
            {"BFP" , SaveType::EEPROM_4k}, // EEPROM_V124     Futari ha Precure Arienaai Yume no Kuni ha Daimeikyuu
            {"BFM" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Futari wa Precure Max Heart Maji! Maji! Fight de IN Janai
            {"BFT" , SaveType::Flash_1M_Macronix}, // FLASH_V102      F-Zero - Climax
            {"BFZ" , SaveType::SRAM_256K}, // SRAM_V113       F-Zero - GP Legends
            {"AFZ" , SaveType::SRAM_256K}, // SRAM_V111       F-Zero - Maximum Velocity
            {"A4X" , SaveType::EEPROM_4k}, // EEPROM_V124     Gachaste! Dino Device 2 Dragon
            {"A4W" , SaveType::EEPROM_4k}, // EEPROM_V124     Gachaste! Dino Device 2 Phoenix
            {"ABI" , SaveType::Flash_512k_SST}, // FLASH_V126      Gachasute! Dino Device - Blue
            {"AAI" , SaveType::Flash_512k_SST}, // FLASH_V126      Gachasute! Dino Device - Red
            {"ANY" , SaveType::SRAM_256K}, // SRAM_V102       Gachinko Pro Yakyuu
            {"AQA" , SaveType::SRAM_256K}, // SRAM_V112       Gadget Racers
            {"AQ2" , SaveType::SRAM_256K}, // SRAM_V102       Gadget Racers
            {"BGH" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Gakkou no Kaidan - Hyakuyobako no Fuuin
            {"AYS" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Gakkou wo Tsukurou!! Advance
            {"BAS" , SaveType::SRAM_256K}, // SRAM_V113       Gakuen Alice - DokiDoki Fushigi Taiken
            {"BGS" , SaveType::EEPROM_4k}, // EEPROM_V124     Gakuen Senki Muryou
            {"AGZ" , SaveType::EEPROM_4k}, // EEPROM_V122     Galaxy Angel GameBoy Advance
            {"AG8" , SaveType::EEPROM_4k}, // EEPROM_V122     Galidor - Defenders of the Outer Dimension
            {"ATY" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Gambler Densetsu Tetsuya - Yomigaeru Densetsu
            {"AQW" , SaveType::SRAM_256K}, // SRAM_V102       Game & Watch Gallery 4
            {"BGW" , SaveType::Flash_1M_Macronix}, // FLASH_V102      Gameboy Wars Advance 1+2
            {"BG7" , SaveType::NONE}, // NONE}            Games Explosion!
            {"BG8" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Ganbare! Dodge Fighters
            {"BGO" , SaveType::NONE}, // NONE}            Garfield - The Search for Pooky
            {"BG9" , SaveType::EEPROM_4k}, // EEPROM_V124     Garfield and His Nine Lives
            {"AYG" , SaveType::EEPROM_4k}, // EEPROM_V122     Gauntlet - Dark Legacy
            {"B69" , SaveType::NONE}, // NONE}            Gauntlet - Rampart
            {"MGU" , SaveType::NONE}, // NONE}            GBA Video - All Grown Up - Volume 1
            {"MCM" , SaveType::NONE}, // NONE}            GBA Video - Cartoon Network Collection - Limited Edition
            {"MCN" , SaveType::NONE}, // NONE}            GBA Video - Cartoon Network Collection - Platinum Edition
            {"MCP" , SaveType::NONE}, // NONE}            GBA Video - Cartoon Network Collection - Premium Edition
            {"MCS" , SaveType::NONE}, // NONE}            GBA Video - Cartoon Network Collection - Special Edition
            {"MCT" , SaveType::NONE}, // NONE}            GBA Video - Cartoon Network Collection - Volume 1
            {"MC2" , SaveType::NONE}, // NONE}            GBA Video - Cartoon Network Collection - Volume 2
            {"MKD" , SaveType::NONE}, // NONE}            GBA Video - Codename Kids Next Door - Volume 1
            {"MDC" , SaveType::NONE}, // NONE}            GBA Video - Disney Channel Collection - Volume 1
            {"MDS" , SaveType::NONE}, // NONE}            GBA Video - Disney Channel Collection - Volume 2
            {"MDR" , SaveType::NONE}, // NONE}            GBA Video - Dora the Explorer - Volume 1
            {"MDB" , SaveType::NONE}, // NONE}            GBA Video - Dragon Ball GT - Volume 1
            {"MNC" , SaveType::NONE}, // NONE}            GBA Video - Nicktoon's Collection - Volume 1
            {"MN2" , SaveType::NONE}, // NONE}            GBA Video - Nicktoons Collection - Volume 2
            {"MN3" , SaveType::NONE}, // NONE}            GBA Video - Nicktoons Volume 3
            {"MPA" , SaveType::NONE}, // NONE}            GBA Video - Pokemon - Volume 1
            {"MPB" , SaveType::NONE}, // NONE}            GBA Video - Pokemon - Volume 2
            {"MPC" , SaveType::NONE}, // NONE}            GBA Video - Pokemon - Volume 3
            {"MPD" , SaveType::NONE}, // NONE}            GBA Video - Pokemon - Volume 4
            {"MSA" , SaveType::NONE}, // NONE}            GBA Video - Shark Tale
            {"MSK" , SaveType::NONE}, // NONE}            GBA Video - Shrek
            {"MST" , SaveType::NONE}, // NONE}            GBA Video - Shrek + Shark Tale
            {"M2S" , SaveType::NONE}, // NONE}            GBA Video - Shrek 2
            {"MSH" , SaveType::NONE}, // NONE}            GBA Video - Sonic X - Volume 1
            {"MSS" , SaveType::NONE}, // NONE}            GBA Video - SpongeBob SquarePants - Volume 1
            {"MS2" , SaveType::NONE}, // NONE}            GBA Video - SpongeBob SquarePants - Volume 2
            {"MS3" , SaveType::NONE}, // NONE}            GBA Video - SpongeBob SquarePants - Volume 3
            {"MSB" , SaveType::NONE}, // NONE}            GBA Video - Strawberry Shortcake - Volume 1
            {"MSR" , SaveType::NONE}, // NONE}            GBA Video - Super Robot Monkey Team - Volume 1
            {"MTM" , SaveType::NONE}, // NONE}            GBA Video - Teenage Mutant Ninja Turtles - Volume 1
            {"MJM" , SaveType::NONE}, // NONE}            GBA Video - The Adventures of Jimmy Neutron Boy Genius - Volume 1
            {"MFO" , SaveType::NONE}, // NONE}            GBA Video - The Fairly Odd Parents - Volume 1
            {"MF2" , SaveType::NONE}, // NONE}            GBA Video - The Fairly Odd Parents - Volume 2
            {"MFP" , SaveType::NONE}, // NONE}            GBA Video - The Proud Family - Volume 1
            {"MYG" , SaveType::NONE}, // NONE}            GBA Video - Yu-Gi-Oh! - Yugi vs. Joey - Volume 1
            {"AGB" , SaveType::NONE}, // NONE}            GBA-SP AV Adaptor
            {"BGK" , SaveType::SRAM_256K}, // SRAM_V112       Gegege no Kitarou - Kikiippatsu! Youkai Rettou
            {"AGE" , SaveType::NONE}, // NONE}            Gekido Advance - Kintaro's Revenge
            {"ANN" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Gekitou Densetsu Noah - Dream Management
            {"AZS" , SaveType::EEPROM_4k}, // EEPROM_V122     Gem Smashers
            {"BGJ" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Genseishin the Justirisers - Souchaku Chikyuu no
            {"BGM" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Gensou Maden Saiyuuki - Hangyaku no Toushin-taishi
            {"AGK" , SaveType::NONE}, // NONE}            Gensou Suikoden - Card Stories
            {"BGI" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Get Ride AMDriver - Senkou no Hero Tanjou
            {"BGP" , SaveType::SRAM_256K}, // SRAM_V113       Get Ride! AMDrive Shutsugeki Battle Party
            {"BGB" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Get! - Boku no Mushi Tsukamaete
            {"BGF" , SaveType::SRAM_256K}, // SRAM_V113       GetBackers Dakkanya - Jagan Fuuin!
            {"A8G" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      GetBackers Dakkanya - Metropolis Dakkan Sakusen!
            {"BR8" , SaveType::EEPROM_4k}, // EEPROM_V124     Ghost Rider
            {"AGV" , SaveType::EEPROM_4k}, // EEPROM_V122     Ghost Trap
            {"B3Z" , SaveType::EEPROM_4k}, // EEPROM_V124     Global Star - Suduoku Feber
            {"AGQ" , SaveType::EEPROM_4k}, // EEPROM_V122     Go! Go! Beckham! - Adventure on Soccer Island
            {"AG4" , SaveType::NONE}, // NONE}            Godzilla - Domination!
            {"AGN" , SaveType::EEPROM_4k_alt}, // EEPROM_V121     Goemon - New Age Shutsudou!
            {"BGG" , SaveType::NONE}, // NONE}            Golden Nugget Casino
            {"AGS" , SaveType::Flash_512k_Atmel}, // FLASH_V123      Golden Sun
            {"AGF" , SaveType::Flash_512k_Atmel}, // FLASH_V123      Golden Sun - The Lost Age
            {"AGA" , SaveType::EEPROM_4k}, // EEPROM_V122     Gradius Galaxies
            {"BGT" , SaveType::EEPROM_4k}, // EEPROM_V124     Grand Theft Auto Advance
            {"AG9" , SaveType::SRAM_256K}, // SRAM_V102       Greatest Nine
            {"BUS" , SaveType::EEPROM_4k}, // EEPROM_V122     Green Eggs and Ham by Dr. Seuss
            {"BGQ" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Greg Hastings' Tournament Paintball Max'd
            {"AGG" , SaveType::NONE}, // NONE}            Gremlins - Stripe vs Gizmo
            {"ARV" , SaveType::SRAM_256K}, // SRAM_V112       Groove Adventure Rave - Hikari to Yami no Daikessen
            {"ARI" , SaveType::SRAM_256K}, // SRAM_V112       Groove Adventure Rave - Hikari to Yami no Daikessen 2
            {"ACA" , SaveType::NONE}, // NONE}            GT Advance - Championship Racing
            {"AGW" , SaveType::EEPROM_4k}, // EEPROM_V122     GT Advance 2 - Rally Racing
            {"A2G" , SaveType::EEPROM_4k}, // EEPROM_V122     GT Advance 3 - Pro Concept Racing
            {"BJA" , SaveType::NONE}, // NONE}            GT Racers
            {"AGX" , SaveType::Flash_512k_SST}, // FLASH_V124      Guilty Gear X - Advance Edition
            {"BGV" , SaveType::EEPROM_4k}, // EEPROM_V124     Gumby vs. The Astrobots
            {"BHG" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Gunstar Super Heroes
            {"BGX" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Gunstar Super Heroes
            {"AIB" , SaveType::SRAM_256K}, // SRAM_V112       Guranbo
            {"AGC" , SaveType::Flash_512k_SST}, // FLASH_V126      Guru Logic Champ
            {"ASB" , SaveType::SRAM_256K}, // SRAM_V112       Gyakuten Saiban
            {"A3G" , SaveType::SRAM_256K}, // SRAM_V112       Gyakuten Saiban 2
            {"A3J" , SaveType::SRAM_256K}, // SRAM_V112       Gyakuten Saiban 3
            {"A8E" , SaveType::EEPROM_4k}, // EEPROM_V122     Hachiemon
            {"BHR" , SaveType::EEPROM_4k}, // EEPROM_V124     Hagane no Renkinjutsushi - Meisou no Rondo
            {"BH2" , SaveType::EEPROM_4k}, // EEPROM_V120     Hagane no Renkinjutsushi - Omoide no Sonata
            {"A2H" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Hajime no Ippo - The Fighting!
            {"AM7" , SaveType::SRAM_256K}, // SRAM_V102       Hamepane - Tokyo Mew Mew
            {"A4K" , SaveType::SRAM_256K}, // SRAM_V113       Hamster Club 4
            {"AHB" , SaveType::SRAM_256K}, // SRAM_V110       Hamster Monogatari 2 GBA
            {"A83" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Hamster Monogatari 3 GBA
            {"BHS" , SaveType::EEPROM_4k}, // EEPROM_V124     Hamster Monogatari 3EX 4 Special
            {"BHC" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Hamster Monogatari Collection
            {"A82" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Hamster Paradise - Pure Heart
            {"AHA" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Hamster Paradise Advance
            {"B85" , SaveType::SRAM_256K}, // SRAM_V113       Hamtaro - Ham-Ham Games
            {"AH3" , SaveType::SRAM_256K}, // SRAM_V103       Hamtaro - Ham-Ham Heartbreak
            {"A84" , SaveType::SRAM_256K}, // SRAM_V113       Hamtaro - Rainbow Rescue
            {"BHA" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Hanabi Hyakkei Advance
            {"ADY" , SaveType::EEPROM_4k}, // EEPROM_V122     Hanafuda Trump Mahjong - Depachika Wayouchuu
            {"BH3" , SaveType::EEPROM_4k_alt}, // EEPROM_V126     Happy Feet
            {"AH6" , SaveType::EEPROM_4k}, // EEPROM_V122     Hardcore Pinball
            {"BHO" , SaveType::NONE}, // NONE}            Hardcore Pool
            {"BHN" , SaveType::NONE}, // NONE}            Harlem Globetrotters - World Tour
            {"BH6" , SaveType::EEPROM_4k}, // EEPROM_V124     Haro no Puyo Puyo
            {"AHQ" , SaveType::SRAM_256K}, // SRAM_V112       Harobots - Robo Hero Battling!!
            {"BHP" , SaveType::EEPROM_4k}, // EEPROM_V122     Harry Potter - Quidditch World Cup
            {"A7H" , SaveType::EEPROM_4k}, // EEPROM_V122     Harry Potter and the Chamber of Secrets
            {"BH8" , SaveType::EEPROM_4k_alt}, // EEPROM_V126     Harry Potter and the Goblet of Fire
            {"BJX" , SaveType::EEPROM_4k}, // EEPROM_V124     Harry Potter and the Order of the Phoenix
            {"BHT" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Harry Potter and the Prisoner of Azkaban
            {"AHR" , SaveType::EEPROM_4k}, // EEPROM_V122     Harry Potter and the Sorcerer's Stone
            {"BJP" , SaveType::EEPROM_8k}, // EEPROM_V126     Harry Potter Collection
            {"ARN" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Harukanaru Toki no Naka de
            {"A4N" , SaveType::SRAM_256K}, // SRAM_V113       Harvest Moon - Friends of Mineral Town
            {"BFG" , SaveType::SRAM_256K}, // SRAM_V113       Harvest Moon - More Friends of Mineral Town
            {"AHS" , SaveType::SRAM_256K}, // SRAM_V110       Hatena Satena
            {"BHJ" , SaveType::EEPROM_4k}, // EEPROM_V124     Heidi
            {"BHD" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Hello Idol Debut
            {"B86" , SaveType::NONE}, // NONE}            Hello Kitty - Happy Party Pals
            {"AKT" , SaveType::EEPROM_4k_alt}, // EEPROM_V121     Hello Kitty Collection - Miracle Fashion Maker
            {"B8F" , SaveType::EEPROM_4k}, // EEPROM_V124     Herbie Fully Loaded
            {"AAE" , SaveType::NONE}, // NONE}            Hey Arnold! - The Movie
            {"BHH" , SaveType::EEPROM_4k}, // EEPROM_V124     Hi Hi Puffy - AmiYumi Kaznapped
            {"AHZ" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Higanbana
            {"ASS" , SaveType::EEPROM_4k}, // EEPROM_V122     High Heat - Major League Baseball 2002
            {"AHH" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     High Heat - Major League Baseball 2003
            {"AHX" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     High Heat - Major League Baseball 2003
            {"BJ2" , SaveType::EEPROM_4k}, // EEPROM_V124     High School Musical - Livin' the Dream
            {"AHK" , SaveType::SRAM_256K}, // SRAM_V102       Hikaru no Go
            {"AHT" , SaveType::NONE}, // NONE}            Hikaru no Go - Taikenban
            {"AKE" , SaveType::SRAM_256K}, // SRAM_V102       Hikaru no Go 2
            {"A3H" , SaveType::SRAM_256K}, // SRAM_V112       Hime Kishi Monogatari - Princess Blue
            {"AHI" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Hitsuji no Kimochi
            {"BHM" , SaveType::NONE}, // NONE}            Home on the Range
            {"BYP" , SaveType::EEPROM_8k}, // EEPROM_V124     Horse & Pony - Let's Ride 2
            {"BHU" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Horsez
            {"AHP" , SaveType::EEPROM_4k}, // EEPROM_V120     Hot Potato!
            {"AHW" , SaveType::EEPROM_4k}, // EEPROM_V121     Hot Wheels - Burnin' Rubber
            {"BHE" , SaveType::NONE}, // NONE}            Hot Wheels - Stunt Track Challenge
            {"AH8" , SaveType::NONE}, // NONE}            Hot Wheels - Velocity X
            {"BHW" , SaveType::NONE}, // NONE}            Hot Wheels - World Race
            {"BHX" , SaveType::NONE}, // NONE}            Hot Wheels All Out
            {"B7I" , SaveType::NONE}, // NONE}            Hudson Best Collection 1
            {"B74" , SaveType::NONE}, // NONE}            Hudson Best Collection Vol. 4 - Nazotoki Collection
            {"B75" , SaveType::NONE}, // NONE}            Hudson Best Collection Vol. 5 - Shooting Collection
            {"B76" , SaveType::NONE}, // NONE}            Hudson Best Collection Vol. 6 - Bouken Jima Collection
            {"B72" , SaveType::NONE}, // NONE}            Hudson Collection Vol. 2 - Lode Runner Collection
            {"B73" , SaveType::NONE}, // NONE}            Hudson Collection Vol. 3 - Action Collection
            {"AZH" , SaveType::NONE}, // NONE}            Hugo - Bukkazoom!
            {"AHJ" , SaveType::EEPROM_4k}, // EEPROM_V124     Hugo - The Evil Mirror
            {"B2H" , SaveType::EEPROM_4k}, // EEPROM_V124     Hugo 2 in 1 - Bukkazoom! + The Evil Mirror
            {"A8N" , SaveType::SRAM_256K}, // SRAM_V112       Hunter X Hunter - Minna Tomodachi Daisakusen!!
            {"A4C" , SaveType::EEPROM_4k}, // EEPROM_V122     I Spy Challenger!
            {"AIA" , SaveType::NONE}, // NONE}            Ice Age
            {"BIA" , SaveType::EEPROM_4k}, // EEPROM_V124     Ice Age 2 - The Meltdown
            {"AR3" , SaveType::EEPROM_4k}, // EEPROM_V120     Ice Nine
            {"BIV" , SaveType::NONE}, // NONE}            Ignition Collection - Volume 1 (3 Games in 1)
            {"AIN" , SaveType::SRAM_256K}, // SRAM_V112       Initial D - Another Stage
            {"AIG" , SaveType::NONE}, // NONE}            Inspector Gadget - Advance Mission
            {"AIR" , SaveType::NONE}, // NONE}            Inspector Gadget Racing
            {"AIK" , SaveType::NONE}, // NONE}            International Karate Advanced
            {"A3K" , SaveType::NONE}, // NONE}            International Karate Plus
            {"AIS" , SaveType::EEPROM_4k_alt}, // EEPROM_V120     International Superstar Soccer
            {"AY2" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     International Superstar Soccer Advance
            {"AI9" , SaveType::SRAM_256K}, // SRAM_V112       Inukko Club
            {"AIY" , SaveType::SRAM_256K}, // SRAM_V112       Inuyasha - Naraku no Wana! Mayoi no Mori no Shoutaijou
            {"AIV" , SaveType::NONE}, // NONE}            Invader
            {"AI3" , SaveType::NONE}, // NONE}            Iridion 3D
            {"AI2" , SaveType::NONE}, // NONE}            Iridion II
            {"BIR" , SaveType::EEPROM_4k}, // EEPROM_V124     Iron Kid
            {"AXT" , SaveType::EEPROM_4k}, // EEPROM_V122     Island Xtreme Stunts
            {"AIE" , SaveType::SRAM_256K}, // SRAM_V112       Isseki Hattyou - Kore 1ppon de 8syurui!
            {"BPI" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     It's Mr Pants
            {"A2J" , SaveType::EEPROM_4k_alt}, // EEPROM_V121     J.League - Winning Eleven Advance 2002
            {"AJP" , SaveType::SRAM_256K}, // SRAM_V110       J.League Pocket
            {"AJ2" , SaveType::SRAM_256K}, // SRAM_V112       J.League Pocket 2
            {"AC2" , SaveType::SRAM_256K}, // SRAM_V112       J.League Pro Soccer Club wo Tsukurou! Advance
            {"AJC" , SaveType::EEPROM_4k}, // EEPROM_V120     Jackie Chan Adventures - Legend of the Darkhand
            {"BNJ" , SaveType::EEPROM_4k}, // EEPROM_V122     Jajamaru Jr. Denshouki - Jaleco Memorial
            {"A7O" , SaveType::EEPROM_4k}, // EEPROM_V122     James Bond 007 - NightFire
            {"AJD" , SaveType::NONE}, // NONE}            James Pond - Codename Robocod
            {"AJJ" , SaveType::EEPROM_4k}, // EEPROM_V120     Jazz Jackrabbit
            {"AJR" , SaveType::EEPROM_4k}, // EEPROM_V122     Jet Set Radio
            {"AGM" , SaveType::Flash_512k_SST}, // FLASH_V124      JGTO Kounin Golf Master Mobile - Japan Golf Tour Game
            {"AJW" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Jikkyou World Soccer Pocket
            {"AJK" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Jikkyou World Soccer Pocket 2
            {"AJN" , SaveType::NONE}, // NONE}            Jimmy Neutron - Boy Genius
            {"BJY" , SaveType::EEPROM_4k}, // EEPROM_V124     Jimmy Neutron Boy Genius - Attack of the Twonkies
            {"AZG" , SaveType::SRAM_256K}, // SRAM_V112       Jinsei Game Advance
            {"AJU" , SaveType::Flash_512k_SST}, // FLASH_V126      Jissen Pachi-Slot Hisshouhou! - Juuou Advance
            {"AJM" , SaveType::EEPROM_4k}, // EEPROM_V121     Jonny Moseley Mad Trix
            {"BJK" , SaveType::EEPROM_4k}, // EEPROM_V124     Juka and the Monophonic Menace
            {"AJQ" , SaveType::EEPROM_4k}, // EEPROM_V122     Jurassic Park III - Island Attack
            {"AJ3" , SaveType::SRAM_256K}, // SRAM_V102       Jurassic Park III - Park Builder
            {"ADN" , SaveType::EEPROM_4k}, // EEPROM_V121     Jurassic Park III - The DNA Factor
            {"AJ8" , SaveType::EEPROM_4k}, // EEPROM_V122     Jurassic Park Institute Tour - Dinosaur Rescue
            {"AJL" , SaveType::EEPROM_4k}, // EEPROM_V122     Justice League - Injustice for All
            {"BJL" , SaveType::EEPROM_4k}, // EEPROM_V120     Justice League Chronicles
            {"BJH" , SaveType::EEPROM_4k}, // EEPROM_V124     Justice League Heroes - The Flash
            {"AKV" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     K-1 Pocket Grand Prix
            {"A2O" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     K-1 Pocket Grand Prix 2
            {"AKD" , SaveType::SRAM_256K}, // SRAM_V102       Kaeru B Back
            {"BKO" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Kaiketsu Zorori to Mahou no Yuuenchi
            {"AKZ" , SaveType::Flash_512k_SST}, // FLASH_V126      Kamaitachi no Yoru Advance
            {"AG2" , SaveType::SRAM_256K}, // SRAM_V112       Kami no Kijutsu - Illusion of the Evil Eyes
            {"AKK" , SaveType::NONE}, // NONE}            Kao the Kangaroo
            {"BK8" , SaveType::SRAM_256K}, // SRAM_V113       Kappa no Kai-Kata - Katan Daibouken
            {"AYK" , SaveType::NONE}, // NONE}            Karnaaj Rally
            {"AN5" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Kawa no Nushi Tsuri 5 - Fushigi no Mori Kara
            {"BN4" , SaveType::SRAM_256K}, // SRAM_V113       Kawa no Nushitsuri 3 & 4
            {"BKG" , SaveType::NONE}, // NONE}            Kawaii Pet Game Gallery
            {"BKP" , SaveType::EEPROM_4k}, // EEPROM_V124     Kawaii Pet Game Gallery 2
            {"A63" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Kawaii Pet Shop Monogatari 3
            {"ATP" , SaveType::Flash_512k_SST}, // FLASH_V126      Keitai Denjuu Telefang 2 - Power
            {"ATS" , SaveType::Flash_512k_SST}, // FLASH_V126      Keitai Denjuu Telefang 2 - Speed
            {"AS3" , SaveType::EEPROM_4k}, // EEPROM_V122     Kelly Slater's Pro Surfer
            {"BKJ" , SaveType::SRAM_256K}, // SRAM_V113       Keroro Gunsou Taiketsu Gekisou
            {"BG2" , SaveType::EEPROM_4k}, // EEPROM_V124     Kessakusen Ganbare Goemon 1 and 2
            {"BYL" , SaveType::EEPROM_4k}, // EEPROM_V124     Kid Paddle
            {"B42" , SaveType::EEPROM_4k}, // EEPROM_V124     Kidou Senshi Gundam Seed Destiny
            {"AAL" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Kidou Tenshi Angelic Layer - Misaki to Yume no Tenshi-tachi
            {"BCX" , SaveType::NONE}, // NONE}            Kid's Cards
            {"XXX" , SaveType::NONE}, // NONE}            Kien
            {"AKI" , SaveType::NONE}, // NONE}            Kiki KaiKai Advance
            {"BKH" , SaveType::EEPROM_4k}, // EEPROM_V124     Kill Switch
            {"B3L" , SaveType::NONE}, // NONE}            Killer 3D Pool
            {"AEY" , SaveType::NONE}, // NONE}            Kim Possible - Revenge of Monkey Fist
            {"BKM" , SaveType::EEPROM_4k}, // EEPROM_V124     Kim Possible 2 - Drakken's Demise
            {"BQP" , SaveType::EEPROM_4k}, // EEPROM_V124     Kim Possible 3 - Team Possible
            {"B8C" , SaveType::SRAM_256K}, // SRAM_V103       Kingdom Hearts - Chain of Memories
            {"AK5" , SaveType::SRAM_256K}, // SRAM_V112       Kinniku Banzuke - Kimero! Kiseki no Kanzen Seiha
            {"AK4" , SaveType::SRAM_256K}, // SRAM_V112       Kinniku Banzuke - Kongou-kun no Daibouken!
            {"A7K" , SaveType::SRAM_256K}, // SRAM_V112       Kirby - Nightmare in Dream Land
            {"B8K" , SaveType::SRAM_256K}, // SRAM_V113       Kirby & the Amazing Mirror
            {"BWA" , SaveType::EEPROM_4k}, // EEPROM_V124     Kisekae Angel Series 1 - Wannyan Aidoru Gakuen
            {"BE2" , SaveType::EEPROM_4k}, // EEPROM_V124     Kisekae Angel Series 2 - Charisma Tenin Ikusei Game
            {"A2V" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Kisekko Gurumi - Chesty to Nuigurumi-tachi no Mahou no Bouken
            {"B2K" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Kiss x Kiss - Seirei Gakuen
            {"AKM" , SaveType::SRAM_256K}, // SRAM_V112       Kiwame Mahjong Deluxe - Mirai Senshi 21
            {"AKL" , SaveType::EEPROM_4k}, // EEPROM_V121     Klonoa - Empire of Dreams
            {"AN6" , SaveType::EEPROM_4k}, // EEPROM_V124     Klonoa 2 - Dream Champ Tournament
            {"AK7" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Klonoa Heroes - Densetsu no Star Medal
            {"BDI" , SaveType::EEPROM_4k}, // EEPROM_V122     Koinu to Issho - Aijou Monogatari
            {"BI2" , SaveType::EEPROM_4k}, // EEPROM_V124     Koinu to Issho! 2
            {"BIS" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Koinu-Chan no Hajimete no Osanpo - Koinu no Kokoro Ikusei Game
            {"AKC" , SaveType::NONE}, // NONE}            Konami Collector's Series - Arcade Advanced
            {"AKW" , SaveType::SRAM_256K}, // SRAM_V111       Konami Krazy Racers
            {"BQB" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Konchu Monster Battle Master
            {"BQS" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Konchu Monster Battle Stadium
            {"BQK" , SaveType::EEPROM_4k}, // EEPROM_V124     Konchuu no Mori no Daibouken
            {"BK7" , SaveType::NONE}, // NONE}            Kong - King of Atlantis
            {"BKQ" , SaveType::EEPROM_4k}, // EEPROM_V124     Kong - The 8th Wonder of the World
            {"AKQ" , SaveType::NONE}, // NONE}            Kong - The Animated Series
            {"BKE" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Konjiki no Gashbell - The Card Battle for GBA
            {"BKB" , SaveType::SRAM_256K}, // SRAM_V103       Konjiki no Gashbell!! Makai no Bookmark
            {"BGY" , SaveType::EEPROM_4k}, // EEPROM_V124     Konjiki no Gashbell!! Unare Yuujou no Dengeki
            {"BUD" , SaveType::EEPROM_4k}, // EEPROM_V124     Konjiki no Gashbell!! Yuujou no Dengeki Dream Tag Tournament
            {"KHP" , SaveType::EEPROM_4k}, // EEPROM_V122     Koro Koro Puzzle - Happy Panechu!
            {"BK5" , SaveType::SRAM_256K}, // SRAM_V103       Korokke Great Toki no Boukensha
            {"A8M" , SaveType::SRAM_256K}, // SRAM_V102       Kotoba no Puzzle - Mojipittan Advance
            {"BK6" , SaveType::SRAM_256K}, // SRAM_V113       Kouchu Ouja - Mushi King
            {"A54" , SaveType::SRAM_256K}, // SRAM_V112       Koukou Juken Advance Series Eigo Koubun Hen - 26 Units Shuuroku
            {"A53" , SaveType::SRAM_256K}, // SRAM_V112       Koukou Juken Advance Series Eijukugo Hen - 650 Phrases Shuuroku
            {"A52" , SaveType::SRAM_256K}, // SRAM_V112       Koukou Juken Advance Series Eitango Hen - 2000 Words Shuuroku
            {"B9A" , SaveType::NONE}, // NONE}            Kunio kun Nekketsu Collection 1
            {"B9B" , SaveType::NONE}, // NONE}            Kunio Kun Nekketsu Collection 2
            {"B9C" , SaveType::NONE}, // NONE}            Kuniokun Nekketsu Collection 3
            {"AGO" , SaveType::SRAM_256K}, // SRAM_V112       Kurohige no Golf Shiyouyo
            {"AKU" , SaveType::SRAM_256K}, // SRAM_V112       Kurohige no Kurutto Jintori
            {"AKR" , SaveType::SRAM_256K}, // SRAM_V111       Kurukuru Kururin
            {"A9Q" , SaveType::SRAM_256K}, // SRAM_V112       Kururin Paradise
            {"ALD" , SaveType::EEPROM_4k}, // EEPROM_V122     Lady Sia
            {"AVD" , SaveType::SRAM_256K}, // SRAM_V112       Legend of Dynamic - Goushouden - Houkai no Rondo
            {"A2L" , SaveType::EEPROM_4k}, // EEPROM_V122     Legends of Wrestling II
            {"BLV" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Legendz - Sign of Necrom
            {"BLJ" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Legendz - Yomigaeru Shiren no Shima
            {"ALB" , SaveType::SRAM_256K}, // SRAM_V112       LEGO Bionicle
            {"AL2" , SaveType::SRAM_256K}, // SRAM_V112       LEGO Island 2 - The Brickster's Revenge
            {"BKN" , SaveType::NONE}, // NONE}            LEGO Knights Kingdom
            {"ALR" , SaveType::EEPROM_4k}, // EEPROM_V120     LEGO Racers 2
            {"BLW" , SaveType::EEPROM_4k}, // EEPROM_V124     LEGO Star Wars - The Video Game
            {"BL7" , SaveType::EEPROM_4k}, // EEPROM_V124     LEGO Star Wars II - The Original Trilogy
            {"BLY" , SaveType::EEPROM_4k}, // EEPROM_V124     Lemony Snicket's A Series of Unfortunate Events
            {"BEF" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Let's Ride - Friends Forever
            {"B34" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Let's Ride! - Sunshine Stables
            {"BL9" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Let's Ride! Dreamer
            {"BRN" , SaveType::SRAM_256K}, // SRAM_V113       Licca-Chan no Oshare Nikki
            {"BRP" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Liliput Oukoku
            {"ALT" , SaveType::NONE}, // NONE}            Lilo & Stitch
            {"BLS" , SaveType::EEPROM_4k}, // EEPROM_V124     Lilo & Stitch 2 - Hamsterveil Havoc
            {"ALQ" , SaveType::SRAM_256K}, // SRAM_V102       Little Buster Q
            {"BEI" , SaveType::NONE}, // NONE}            Little Einsteins
            {"ALC" , SaveType::NONE}, // NONE}            Little League Baseball 2002
            {"BLI" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Little Patissier Cake no Oshiro
            {"BLM" , SaveType::EEPROM_4k_alt}, // EEPROM_V120     Lizzie McGuire
            {"BL2" , SaveType::EEPROM_4k}, // EEPROM_V124     Lizzie McGuire 2 - Lizzie Diaries
            {"BL4" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Lizzie McGuire 2 - Lizzie Diaries - Special Edition
            {"BL3" , SaveType::EEPROM_4k}, // EEPROM_V124     Lizzie McGuire 3 - Homecoming Havoc
            {"A39" , SaveType::EEPROM_4k}, // EEPROM_V122     Lode Runner
            {"BLT" , SaveType::EEPROM_4k}, // EEPROM_V122     Looney Tunes - Back in Action
            {"BLN" , SaveType::EEPROM_4k}, // EEPROM_V124     Looney Tunes Double Pack - Dizzy Driving + Acme Antics
            {"ALH" , SaveType::EEPROM_4k}, // EEPROM_V121     Love Hina Advance - Shukufuku no Kane ha Naru Kana
            {"ALL" , SaveType::NONE}, // NONE}            Lucky Luke - Wanted!
            {"AGD" , SaveType::SRAM_256K}, // SRAM_V102       Lufia - The Ruins of Lore
            {"ALN" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Lunar Legend
            {"AML" , SaveType::EEPROM_4k}, // EEPROM_V122     M&M's Blast!
            {"BEM" , SaveType::NONE}, // NONE}            M&M's Break' Em
            {"BGZ" , SaveType::EEPROM_4k}, // EEPROM_V124     Madagascar
            {"BM7" , SaveType::EEPROM_4k}, // EEPROM_V124     Madagascar - Operation Penguin
            {"B6M" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Madden NFL 06
            {"B7M" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Madden NFL 07
            {"A2M" , SaveType::EEPROM_4k}, // EEPROM_V122     Madden NFL 2002
            {"ANJ" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Madden NFL 2003
            {"BMD" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Madden NFL 2004
            {"BMF" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Madden NFL 2005
            {"A2I" , SaveType::EEPROM_4k_alt}, // EEPROM_V120     Magi Nation
            {"AJO" , SaveType::Flash_512k_SST}, // FLASH_V126      Magical Houshin
            {"AQM" , SaveType::EEPROM_4k}, // EEPROM_V122     Magical Quest 2 Starring Mickey & Minnie
            {"BMQ" , SaveType::EEPROM_4k}, // EEPROM_V124     Magical Quest 3 Starring Mickey and Donald
            {"A3M" , SaveType::EEPROM_4k}, // EEPROM_V122     Magical Quest Starring Mickey & Minnie
            {"AMV" , SaveType::Flash_512k_SST}, // FLASH_V126      Magical Vacation
            {"AMP" , SaveType::SRAM_256K}, // SRAM_V112       Mahjong Police
            {"BNG" , SaveType::SRAM_256K}, // SRAM_V113       Mahou Sensei Negima
            {"BNM" , SaveType::SRAM_256K}, // SRAM_V103       Mahou Sensei Negima! Private Lesson 2
            {"AMC" , SaveType::Flash_512k_Atmel}, // FLASH_V121      Mail de Cute
            {"B2Y" , SaveType::NONE}, // NONE}            Major League Baseball 2k7
            {"ACO" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Manga-ka Debut Monogatari
            {"A4M" , SaveType::NONE}, // NONE}            Manic Miner
            {"B68" , SaveType::NONE}, // NONE}            Marble Madness - Klax
            {"BQL" , SaveType::NONE}, // NONE}            March of the Penguins
            {"BM9" , SaveType::SRAM_256K}, // SRAM_V103       Marheaven - Knockin' on Heaven's Door
            {"ANS" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Marie, Elie & Anis no Atelier - Soyokaze Kara no Dengon
            {"A88" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Mario & Luigi - Superstar Saga
            {"BMG" , SaveType::SRAM_256K}, // SRAM_V113       Mario Golf - Advance Tour
            {"AMK" , SaveType::Flash_512k_SST}, // FLASH_V124      Mario Kart - Super Circuit
            {"B8M" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Mario Party Advance
            {"BMV" , SaveType::EEPROM_4k}, // EEPROM_V124     Mario Pinball Land
            {"BTM" , SaveType::SRAM_256K}, // SRAM_V113       Mario Tennis - Power Tour
            {"BM5" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Mario vs. Donkey Kong
            {"B4M" , SaveType::EEPROM_4k}, // EEPROM_V124     Marvel - Ultimate Alliance
            {"AKS" , SaveType::EEPROM_4k}, // EEPROM_V120     Mary-Kate and Ashley - Girls Night Out
            {"AAY" , SaveType::NONE}, // NONE}            Mary-Kate and Ashley Sweet 16 - Licensed to Drive
            {"AGU" , SaveType::NONE}, // NONE}            Masters of the Universe - He-Man Power of Grayskull
            {"AHO" , SaveType::EEPROM_4k}, // EEPROM_V122     Mat Hoffman's Pro BMX
            {"AH2" , SaveType::EEPROM_4k}, // EEPROM_V122     Mat Hoffman's Pro BMX 2
            {"BMR" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Matantei Loki Ragnarok - Gensou no Labyrinth
            {"ARQ" , SaveType::NONE}, // NONE}            Matchbox Cross Town Heroes
            {"BIY" , SaveType::EEPROM_4k}, // EEPROM_V124     Math Patrol - The Kleptoid Threat
            {"BME" , SaveType::EEPROM_4k}, // EEPROM_V124     Max Payne
            {"BEE" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Maya The Bee - Sweet Gold
            {"ABV" , SaveType::NONE}, // NONE}            Maya the Bee - The Great Adventure
            {"BFQ" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Mazes of Fate
            {"AKG" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Mech Platoon
            {"A8B" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Medabots - Metabee Version
            {"A9B" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Medabots - Rokusho Version
            {"AK8" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Medabots AX - Metabee Version
            {"AK9" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Medabots AX - Rokusho Version
            {"BMH" , SaveType::EEPROM_4k}, // EEPROM_V120     Medal of Honor - Infiltrator
            {"AUG" , SaveType::NONE}, // NONE}            Medal of Honor - Underground
            {"A5K" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Medarot 2 Core - Kabuto Version
            {"A5Q" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Medarot 2 Core - Kuwagata Version
            {"AGH" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Medarot G - Kabuto Version
            {"AGI" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Medarot G - Kuwagata Version
            {"ANA" , SaveType::Flash_512k_SST}, // FLASH_V124      Medarot Navi - Kabuto
            {"AVI" , SaveType::Flash_512k_SST}, // FLASH_V124      Medarot Navi - Kuwagata
            {"BRH" , SaveType::EEPROM_4k}, // EEPROM_V124     Meet the Robinsons
            {"A89" , SaveType::SRAM_256K}, // SRAM_V113       MegaMan - Battle Chip Challenge
            {"A6M" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     MegaMan & Bass
            {"ARE" , SaveType::SRAM_256K}, // SRAM_V112       MegaMan Battle Network
            {"AE2" , SaveType::SRAM_256K}, // SRAM_V112       MegaMan Battle Network 2
            {"AM2" , SaveType::SRAM_256K}, // SRAM_V112       MegaMan Battle Network 2
            {"A3X" , SaveType::SRAM_256K}, // SRAM_V112       MegaMan Battle Network 3 Blue
            {"A6B" , SaveType::SRAM_256K}, // SRAM_V112       MegaMan Battle Network 3 White
            {"B4B" , SaveType::SRAM_256K}, // SRAM_V113       Megaman Battle Network 4 - Blue Moon
            {"B4W" , SaveType::SRAM_256K}, // SRAM_V113       MegaMan Battle Network 4 - Red Sun
            {"BRK" , SaveType::SRAM_256K}, // SRAM_V113       Megaman Battle Network 5 - Team Colonel
            {"BRB" , SaveType::SRAM_256K}, // SRAM_V113       Megaman Battle Network 5 - Team Protoman
            {"BR6" , SaveType::SRAM_256K}, // SRAM_V113       MegaMan Battle Network 6 - Cybeast Falzar
            {"BR5" , SaveType::SRAM_256K}, // SRAM_V113       MegaMan Battle Network 6 - Cybeast Gregar
            {"AZC" , SaveType::SRAM_256K}, // SRAM_V112       MegaMan Zero
            {"A62" , SaveType::SRAM_256K}, // SRAM_V113       MegaMan Zero 2
            {"BZ3" , SaveType::SRAM_256K}, // SRAM_V113       Megaman Zero 3
            {"B4Z" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Megaman Zero 4
            {"BQV" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Meine Tierarztpraxis
            {"AC4" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Meitantei Conan - Nerawareta Tantei
            {"BQA" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Meitantei Conan Akatsuki no Monument
            {"AMI" , SaveType::NONE}, // NONE}            Men in Black - The Series
            {"B3M" , SaveType::SRAM_256K}, // SRAM_V112       Mermaid Melody - Pichi Pichi Picchi Pichi Pichitto Live Start
            {"BMA" , SaveType::SRAM_256K}, // SRAM_V112       Mermaid Melody - Pichi Pichi Pitch
            {"BM8" , SaveType::SRAM_256K}, // SRAM_V113       Mermaid Melody - Pichi Pichi Pitch Pichi Pichi Party
            {"A9T" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Metal Max 2 - Kai Version
            {"BSM" , SaveType::EEPROM_4k}, // EEPROM_V124     Metal Slug Advance
            {"AAP" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Metalgun Slinger
            {"BMX" , SaveType::SRAM_256K}, // SRAM_V113       Metroid - Zero Mission
            {"AMT" , SaveType::SRAM_256K}, // SRAM_V112       Metroid Fusion
            {"BMK" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Mezase Koushien
            {"BM3" , SaveType::EEPROM_4k}, // EEPROM_V124     Mickey to Donald no Magical Quest 3
            {"A29" , SaveType::EEPROM_4k}, // EEPROM_V122     Mickey to Minnie no Magical Quest 2
            {"BM4" , SaveType::EEPROM_4k}, // EEPROM_V122     Mickey to Pocket Resort
            {"AXZ" , SaveType::NONE}, // NONE}            Micro Machines
            {"AMQ" , SaveType::NONE}, // NONE}            Midnight Club - Street Racing
            {"AM3" , SaveType::NONE}, // NONE}            Midway's Greatest Arcade Hits
            {"BMB" , SaveType::EEPROM_4k}, // EEPROM_V124     Mighty Beanz - Pocket Puzzles
            {"AM6" , SaveType::EEPROM_4k}, // EEPROM_V122     Mike Tyson Boxing
            {"AM9" , SaveType::EEPROM_4k}, // EEPROM_V122     Mike Tyson Boxing
            {"B62" , SaveType::NONE}, // NONE}            Millipede - Super Break Out - Lunar Lander
            {"AOD" , SaveType::EEPROM_4k}, // EEPROM_V122     Minami no Umi no Odyssey
            {"AHC" , SaveType::SRAM_256K}, // SRAM_V112       Minimoni - Mika no Happy Morning Chatty
            {"AOH" , SaveType::EEPROM_4k}, // EEPROM_V122     Minimoni - Onegai Ohoshi-sama!
            {"BMJ" , SaveType::EEPROM_4k}, // EEPROM_V124     Minna no Mahjong
            {"BMO" , SaveType::SRAM_256K}, // SRAM_V103       Minna no Ouji-sama
            {"BKK" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Minna no Shiiku Series - Boku no Kabuto-Kuwagata
            {"AB7" , SaveType::SRAM_256K}, // SRAM_V112       Minna no Shiiku Series 1 - Boku no Kabuto Mushi
            {"AW7" , SaveType::SRAM_256K}, // SRAM_V112       Minna no Shiiku Series 2 - Boku no Kuwagata
            {"BTL" , SaveType::EEPROM_4k}, // EEPROM_V124     Minna no Soft Series - Happy Trump 20
            {"BHY" , SaveType::EEPROM_4k}, // EEPROM_V124     Minna no Soft Series - Hyokkori Hyoutan-jima - Don Gabacho Daikatsuyaku no Maki
            {"BSG" , SaveType::EEPROM_4k}, // EEPROM_V124     Minna no Soft Series - Minna no Shogi
            {"ARM" , SaveType::EEPROM_4k}, // EEPROM_V120     Minority Report
            {"B3I" , SaveType::EEPROM_4k}, // EEPROM_V124     Mirakuru! Panzou - 7 Tsuno Hosh no Kaizoku
            {"AIH" , SaveType::EEPROM_4k}, // EEPROM_V122     Mission Impossible - Operation Surma
            {"A5M" , SaveType::NONE}, // NONE}            MLB SlugFest 20-04
            {"AMB" , SaveType::Flash_512k_SST}, // FLASH_V124      Mobile Pro Yakyuu - Kantoku no Saihai
            {"BGN" , SaveType::NONE}, // NONE}            Mobile Suit Gundam Seed - Battle Assault
            {"BJC" , SaveType::EEPROM_4k}, // EEPROM_V124     Moero! Jaleco Collection
            {"BM2" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Momotarou Densetsu G Gold Deck wo Tsukure!
            {"AMM" , SaveType::SRAM_256K}, // SRAM_V110       Momotarou Matsuri
            {"BUM" , SaveType::NONE}, // NONE}            Monopoly
            {"AM8" , SaveType::EEPROM_4k}, // EEPROM_V122     Monster Force
            {"ANF" , SaveType::SRAM_256K}, // SRAM_V102       Monster Gate
            {"A6G" , SaveType::SRAM_256K}, // SRAM_V102       Monster Gate - Ooinaru Dungeon - Fuuin no Orb
            {"AMN" , SaveType::SRAM_256K}, // SRAM_V110       Monster Guardians
            {"BQ7" , SaveType::EEPROM_4k}, // EEPROM_V124     Monster House
            {"AJA" , SaveType::SRAM_256K}, // SRAM_V112       Monster Jam - Maximum Destruction
            {"AA4" , SaveType::SRAM_256K}, // SRAM_V112       Monster Maker 4 - Flash Card
            {"AA5" , SaveType::SRAM_256K}, // SRAM_V112       Monster Maker 4 - Killer Dice
            {"AMF" , SaveType::SRAM_256K}, // SRAM_V102       Monster Rancher Advance
            {"A2Q" , SaveType::SRAM_256K}, // SRAM_V102       Monster Rancher Advance 2
            {"A3N" , SaveType::SRAM_256K}, // SRAM_V103       Monster Summoner
            {"BMT" , SaveType::EEPROM_4k}, // EEPROM_V120     Monster Truck Madness
            {"BMC" , SaveType::NONE}, // NONE}            Monster Trucks
            {"BYM" , SaveType::NONE}, // NONE}            Monster Trucks Mayhem
            {"A4B" , SaveType::NONE}, // NONE}            Monster! Bass Fishing
            {"AMX" , SaveType::NONE}, // NONE}            Monsters, Inc.
            {"AU3" , SaveType::EEPROM_4k}, // EEPROM_V122     Moorhen 3 - The Chicken Chase!
            {"AMS" , SaveType::SRAM_256K}, // SRAM_V112       Morita Shougi Advance
            {"AXD" , SaveType::EEPROM_4k}, // EEPROM_V122     Mortal Kombat - Deadly Alliance
            {"AW4" , SaveType::EEPROM_4k}, // EEPROM_V122     Mortal Kombat - Tournament Edition
            {"AM5" , SaveType::NONE}, // NONE}            Mortal Kombat Advance
            {"A2U" , SaveType::SRAM_256K}, // SRAM_V112       Mother 1+2
            {"A3U" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Mother 3
            {"AM4" , SaveType::NONE}, // NONE}            Moto GP
            {"A9M" , SaveType::SRAM_256K}, // SRAM_V113       Moto Racer Advance
            {"AMR" , SaveType::EEPROM_4k}, // EEPROM_V122     Motocross Maniacs Advance
            {"BR2" , SaveType::SRAM_256K}, // SRAM_V103       Mr. Driller 2
            {"AD2" , SaveType::SRAM_256K}, // SRAM_V100       Mr. Driller 2
            {"AD5" , SaveType::SRAM_256K}, // SRAM_V102       Mr. Driller A - Fushigi na Pacteria
            {"AZR" , SaveType::NONE}, // NONE}            Mr. Nutz
            {"BPC" , SaveType::EEPROM_4k}, // EEPROM_V124     Ms. Pac-Man - Maze Madness
            {"B6P" , SaveType::EEPROM_4k}, // EEPROM_V124     Ms. Pac-Man - Maze Madness + Pac-Man World
            {"BML" , SaveType::EEPROM_4k}, // EEPROM_V122     Mucha Lucha - Mascaritas of the Lost Code
            {"AG6" , SaveType::SRAM_256K}, // SRAM_V102       Mugenborg
            {"AMW" , SaveType::EEPROM_4k}, // EEPROM_V122     Muppet Pinball Mayhem
            {"AMU" , SaveType::SRAM_256K}, // SRAM_V112       Mutsu - Water Looper Mutsu
            {"A2X" , SaveType::NONE}, // NONE}            MX 2002 - Featuring Ricky Carmichael
            {"BFR" , SaveType::EEPROM_8k}, // EEPROM_V126     My Animal Centre in Africa
            {"BL6" , SaveType::NONE}, // NONE}            My Little Pony - Crystal Princess - The Runaway Rainbow
            {"BQT" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     My Pet Hotel
            {"AKP" , SaveType::SRAM_256K}, // SRAM_V112       Nakayoshi Mahjong - Kaburiichi
            {"AH7" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Nakayoshi Pet Advance Series 1 - Kawaii Hamster
            {"AI7" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Nakayoshi Pet Advance Series 2 - Kawaii Koinu
            {"BKI" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Nakayoshi Pet Advance Series 4 - Kawaii Koinu Mini - Wankoto Asobou!! Kogata-ken
            {"AHV" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Nakayoshi Youchien - Sukoyaka Enji Ikusei Game
            {"ANM" , SaveType::NONE}, // NONE}            Namco Museum
            {"B5N" , SaveType::NONE}, // NONE}            Namco Museum - 50th Anniversary
            {"AND" , SaveType::NONE}, // NONE}            Nancy Drew - Message in a Haunted Mansion
            {"ANP" , SaveType::SRAM_256K}, // SRAM_V110       Napoleon
            {"AYR" , SaveType::SRAM_256K}, // SRAM_V112       Narikiri Jockey Game - Yuushun Rhapsody
            {"AUE" , SaveType::SRAM_256K}, // SRAM_V103       Naruto - Konoha Senki
            {"A7A" , SaveType::EEPROM_4k}, // EEPROM_V124     Naruto - Ninja Council
            {"BN2" , SaveType::SRAM_256K}, // SRAM_V113       Naruto Ninja Council 2
            {"BNR" , SaveType::SRAM_256K}, // SRAM_V113       Naruto RPG - Uketsugareshi Hi no Ishi
            {"ANH" , SaveType::EEPROM_4k}, // EEPROM_V120     NASCAR Heat 2002
            {"AN2" , SaveType::Flash_512k_SST}, // FLASH_V126      Natural 2 - Duo
            {"ABN" , SaveType::NONE}, // NONE}            NBA Jam 2002
            {"BNW" , SaveType::EEPROM_4k}, // EEPROM_V124     Need for Speed - Most Wanted
            {"AZF" , SaveType::EEPROM_4k}, // EEPROM_V124     Need for Speed - Porsche Unleashed
            {"BNS" , SaveType::EEPROM_4k}, // EEPROM_V124     Need for Speed - Underground
            {"BNF" , SaveType::EEPROM_4k}, // EEPROM_V124     Need for Speed - Underground 2
            {"BN7" , SaveType::EEPROM_4k}, // EEPROM_V124     Need for Speed Carbon - Own the City
            {"ABZ" , SaveType::NONE}, // NONE}            NFL Blitz 20-02
            {"ANK" , SaveType::NONE}, // NONE}            NFL Blitz 20-03
            {"ATX" , SaveType::EEPROM_4k}, // EEPROM_V122     NGT - Next Generation Tennis
            {"ANL" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     NHL 2002
            {"AN4" , SaveType::EEPROM_4k}, // EEPROM_V122     NHL Hitz 20-03
            {"BUJ" , SaveType::NONE}, // NONE}            Nicktoons - Attack of the Toybots
            {"BNV" , SaveType::EEPROM_4k}, // EEPROM_V124     Nicktoons - Battle for Volcano Island
            {"BCC" , SaveType::EEPROM_4k}, // EEPROM_V124     Nicktoons - Freeze Frame Frenzy
            {"ANQ" , SaveType::EEPROM_4k}, // EEPROM_V122     Nicktoons Racing
            {"BNU" , SaveType::NONE}, // NONE}            Nicktoons Unite!
            {"ANX" , SaveType::EEPROM_4k}, // EEPROM_V122     Ninja Five-O
            {"ANT" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Nippon Pro Mahjong Renmei Kounin - Tetsuman Advance
            {"AGP" , SaveType::NONE}, // NONE}            No Rules - Get Phat
            {"ANO" , SaveType::SRAM_256K}, // SRAM_V102       Nobunaga Ibun
            {"ANB" , SaveType::Flash_512k_SST}, // FLASH_V125      Nobunaga no Yabou
            {"BNK" , SaveType::NONE}, // NONE}            Noddy - A day in Toyland
            {"BKR" , SaveType::EEPROM_4k}, // EEPROM_V124     Nonono Puzzle Chailien
            {"BNY" , SaveType::EEPROM_4k}, // EEPROM_V124     Nyan Nyan Nyanko no Nyan Collection
            {"BO2" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Ochainu no Bouken Jima
            {"BIK" , SaveType::EEPROM_8k}, // EEPROM_V124     Ochainuken Kururin
            {"BDR" , SaveType::EEPROM_4k}, // EEPROM_V124     Ochaken no Heya
            {"BCU" , SaveType::EEPROM_4k}, // EEPROM_V124     Ochaken no Yumebouken
            {"BOD" , SaveType::NONE}, // NONE}            Oddworld - Munch's Oddysee
            {"A87" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Ohanaya-San Monogatari GBA
            {"BOJ" , SaveType::EEPROM_4k}, // EEPROM_V122     Ojarumaru - Gekkouchou Sanpo de Ojaru
            {"AOK" , SaveType::SRAM_256K}, // SRAM_V112       Okumanchouja Game - Nottori Daisakusen!
            {"BON" , SaveType::EEPROM_4k}, // EEPROM_V124     One Piece
            {"BIP" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     One Piece - Dragon Dream
            {"B08" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     One Piece - Going Baseball
            {"BO8" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     One Piece - Going Baseball Haejeok Yaku
            {"AUS" , SaveType::SRAM_256K}, // SRAM_V112       One Piece - Mezase! King of Berries
            {"AO7" , SaveType::SRAM_256K}, // SRAM_V112       One Piece - Nanatsu Shima no Daihihou
            {"A6O" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Onimusha Tactics
            {"BIT" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Onmyou Taisenki Zeroshik
            {"BOA" , SaveType::EEPROM_4k}, // EEPROM_V124     Open Season
            {"BAA" , SaveType::NONE}, // NONE}            Operation Armored Liberty
            {"AOR" , SaveType::SRAM_256K}, // SRAM_V112       Oriental Blue - Ao no Tengai
            {"AIC" , SaveType::Flash_512k_SST}, // FLASH_V126      Oshaberi Inko Club
            {"AOP" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Oshare Princess
            {"AO2" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Oshare Princess 2
            {"BO3" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Oshare Princess 3
            {"BO5" , SaveType::EEPROM_4k}, // EEPROM_V124     Oshare Princess 5
            {"A5S" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Oshare Wanko
            {"BOF" , SaveType::NONE}, // NONE}            Ottifanten Pinball
            {"BH5" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Over the Hedge
            {"BH7" , SaveType::EEPROM_4k}, // EEPROM_V124     Over the Hedge - Hammy Goes Nuts
            {"BOZ" , SaveType::NONE}, // NONE}            Ozzy & Drix
            {"APC" , SaveType::NONE}, // NONE}            Pac-Man Collection
            {"BP8" , SaveType::NONE}, // NONE}            Pac-Man Pinball Advance
            {"BPA" , SaveType::EEPROM_4k}, // EEPROM_V124     Pac-Man World
            {"B2C" , SaveType::NONE}, // NONE}            Pac-Man World 2
            {"B6B" , SaveType::NONE}, // NONE}            Paperboy - Rampage
            {"BBQ" , SaveType::SRAM_256K}, // SRAM_V113       Pawa Poke Dash
            {"BUR" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Paws & Claws - Pet Resort
            {"BPK" , SaveType::EEPROM_4k}, // EEPROM_V124     Payback
            {"BPZ" , SaveType::EEPROM_4k}, // EEPROM_V122     Pazunin - Uminin No Puzzle de Nimu
            {"APP" , SaveType::NONE}, // NONE}            Peter Pan - Return to Neverland
            {"BPT" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Peter Pan - The Motion Picture Event
            {"AJH" , SaveType::SRAM_256K}, // SRAM_V113       Petz - Hamsterz Life 2
            {"BNB" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Petz Vet
            {"BPV" , SaveType::EEPROM_8k}, // EEPROM_V124     Pferd & Pony - Mein Pferdehof
            {"APX" , SaveType::EEPROM_4k}, // EEPROM_V122     Phalanx - The Enforce Fighter A-144
            {"AYC" , SaveType::EEPROM_4k_alt}, // EEPROM_V120     Phantasy Star Collection
            {"BFX" , SaveType::EEPROM_4k}, // EEPROM_V124     Phil of the Future
            {"BP3" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Pia Carrot he Youkoso!! 3.3
            {"A9N" , SaveType::NONE}, // NONE}            Piglet's Big Game
            {"BPN" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Pika Pika Nurse Monogatari - Nurse Ikusei Game
            {"APZ" , SaveType::EEPROM_4k}, // EEPROM_V122     Pinball Advance
            {"APL" , SaveType::NONE}, // NONE}            Pinball Challenge Deluxe
            {"A2T" , SaveType::NONE}, // NONE}            Pinball Tycoon
            {"APE" , SaveType::NONE}, // NONE}            Pink Panther - Pinkadelic Pursuit
            {"AP7" , SaveType::NONE}, // NONE}            Pink Panther - Pinkadelic Pursuit
            {"API" , SaveType::NONE}, // NONE}            Pinky and the Brain - The Masterplan
            {"APN" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Pinky Monkey Town
            {"APB" , SaveType::SRAM_256K}, // SRAM_V111       Pinobee - Wings of Adventure
            {"AP6" , SaveType::EEPROM_4k}, // EEPROM_V122     Pinobee & Phoebee
            {"B8Q" , SaveType::EEPROM_4k}, // EEPROM_V124     Pirates of the Caribbean - Dead Man's Chest
            {"A8Q" , SaveType::NONE}, // NONE}            Pirates of the Caribbean - The Curse of the Black Pearl
            {"BPH" , SaveType::EEPROM_4k}, // EEPROM_V120     Pitfall - The Lost Expedition
            {"APF" , SaveType::NONE}, // NONE}            Pitfall - The Mayan Adventure
            {"BQ9" , SaveType::EEPROM_4k}, // EEPROM_V124     Pixeline i Pixieland
            {"APM" , SaveType::NONE}, // NONE}            Planet Monsters
            {"AYN" , SaveType::NONE}, // NONE}            Planet of the Apes
            {"ASH" , SaveType::SRAM_256K}, // SRAM_V110       Play Novel - Silent Hill
            {"BTD" , SaveType::EEPROM_4k}, // EEPROM_V124     Pocket Dogs
            {"AP9" , SaveType::SRAM_256K}, // SRAM_V102       Pocket Music
            {"BPJ" , SaveType::EEPROM_4k}, // EEPROM_V124     Pocket Professor - Kwik Notes Vol. 1
            {"APK" , SaveType::NONE}, // NONE}            Pocky & Rocky with Becky
            {"BPE" , SaveType::Flash_1M_Macronix_RTC}, // FLASH_V103      Pokemon - Emerald Version
            {"BPR" , SaveType::Flash_1M_Macronix_RTC}, // FLASH_V103      Pokemon - Fire Red Version
            {"BPG" , SaveType::Flash_1M_Macronix_RTC}, // FLASH_V103      Pokemon - Leaf Green Version
            {"AXV" , SaveType::Flash_1M_Macronix_RTC}, // FLASH_V103      Pokemon - Ruby Version
            {"AXP" , SaveType::Flash_1M_Macronix_RTC}, // FLASH_V103      Pokemon - Sapphire Version
            {"B24" , SaveType::Flash_1M_Macronix}, // FLASH_V102      Pokemon Mystery Dungeon - Red Rescue Team
            {"BPP" , SaveType::SRAM_256K}, // SRAM_V102       Pokemon Pinball - Ruby & Sapphire
            {"BII" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Polarium Advance
            {"B3F" , SaveType::NONE}, // NONE}            Polly Pocket!
            {"AOT" , SaveType::NONE}, // NONE}            Polly! Pocket - Super Splash Island
            {"APO" , SaveType::NONE}, // NONE}            Popeye - Rush for Spinach
            {"BRO" , SaveType::NONE}, // NONE}            Postman Pat and the Greendale Rocket
            {"B8P" , SaveType::SRAM_256K}, // SRAM_V112       Power Pro Kun Pocket 1&2
            {"AP3" , SaveType::SRAM_256K}, // SRAM_V110       Power Pro Kun Pocket 3
            {"AP4" , SaveType::SRAM_256K}, // SRAM_V112       Power Pro Kun Pocket 4
            {"A5P" , SaveType::SRAM_256K}, // SRAM_V112       Power Pro Kun Pocket 5
            {"BP6" , SaveType::SRAM_256K}, // SRAM_V112       Power Pro Kun Pocket 6
            {"BP7" , SaveType::SRAM_256K}, // SRAM_V113       Power Pro Kun Pocket 7
            {"BPO" , SaveType::NONE}, // NONE}            Power Rangers - Dino Thunder
            {"BPW" , SaveType::NONE}, // NONE}            Power Rangers - Ninja Storm
            {"BRD" , SaveType::NONE}, // NONE}            Power Rangers - S.P.D.
            {"APR" , SaveType::NONE}, // NONE}            Power Rangers - Time Force
            {"APW" , SaveType::NONE}, // NONE}            Power Rangers - Wild Force
            {"APH" , SaveType::NONE}, // NONE}            Prehistorik Man
            {"BAQ" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Premier Action Soccer
            {"BPM" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Premier Manager 2003-04
            {"BP4" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Premier Manager 2004 - 2005
            {"BP5" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Premier Manager 2005 - 2006
            {"BPY" , SaveType::EEPROM_4k}, // EEPROM_V122     Prince of Persia - The Sands of Time
            {"B2Q" , SaveType::EEPROM_4k}, // EEPROM_V124     Prince of Persia - The Sands of Time + Tomb Raider - The Prophecy
            {"BNP" , SaveType::NONE}, // NONE}            Princess Natasha - Student Secret Agent
            {"B2O" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Pro Mahjong - Tsuwamono GBA
            {"ALM" , SaveType::SRAM_256K}, // SRAM_V112       Pro Yakyuu Team wo Tsukurou! Advance
            {"APU" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     PukuPuku Tennen Kairanban
            {"BPQ" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     PukuPuku Tennen Kairanban - Koi no Cupid Daisakusen
            {"B3P" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Pukupuku Tennen Kairanban Youkoso Illusion Land
            {"APG" , SaveType::NONE}, // NONE}            Punch King - Arcade Boxing
            {"BYX" , SaveType::EEPROM_4k}, // EEPROM_V124     Puppy Luv - Spa and Resort
            {"APY" , SaveType::EEPROM_4k}, // EEPROM_V122     Puyo Pop
            {"BPF" , SaveType::EEPROM_4k}, // EEPROM_V124     Puyo Pop Fever
            {"AEH" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Puzzle & Tantei Collection
            {"BPB" , SaveType::SRAM_256K}, // SRAM_V113       Pyuu to Fuku! Jaguar - Byo to Deru! Megane Kun
            {"BQD" , SaveType::NONE}, // NONE}            Quad Desert Fury
            {"BRW" , SaveType::NONE}, // NONE}            Racing Fever
            {"BRA" , SaveType::EEPROM_4k}, // EEPROM_V124     Racing Gears Advance
            {"ARX" , SaveType::NONE}, // NONE}            Rampage - Puzzle Attack
            {"BRF" , SaveType::EEPROM_4k}, // EEPROM_V124     Rapala Pro Fishing
            {"BNL" , SaveType::NONE}, // NONE}            Ratatouille
            {"BRM" , SaveType::SRAM_256K}, // SRAM_V113       Rave Master - Special Attack Force
            {"BX5" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Rayman - 10th Anniversary
            {"BRY" , SaveType::EEPROM_4k}, // EEPROM_V124     Rayman - Hoodlum's Revenge
            {"BQ3" , SaveType::EEPROM_4k}, // EEPROM_V124     Rayman - Raving Rabbids
            {"AYZ" , SaveType::EEPROM_4k}, // EEPROM_V122     Rayman 3
            {"ARY" , SaveType::EEPROM_4k}, // EEPROM_V111     Rayman Advance
            {"ARF" , SaveType::NONE}, // NONE}            Razor Freestyle Scooter
            {"AR2" , SaveType::NONE}, // NONE}            Ready 2 Rumble Boxing - Round 2
            {"BRL" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Rebelstar - Tactical Command
            {"ARH" , SaveType::SRAM_256K}, // SRAM_V112       Recca no Honoo - The Game
            {"AR9" , SaveType::NONE}, // NONE}            Reign of Fire
            {"BR9" , SaveType::EEPROM_4k}, // EEPROM_V124     Relaxuma na Mainichi
            {"AQH" , SaveType::NONE}, // NONE}            Rescue Heroes - Billy Blazes!
            {"BRI" , SaveType::SRAM_256K}, // SRAM_V110       Rhythm Tengoku
            {"B66" , SaveType::NONE}, // NONE}            Risk - Battleship - Clue
            {"BDT" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     River City Ransom EX
            {"BRE" , SaveType::SRAM_256K}, // SRAM_V103       Riviera - The Promised Land
            {"A9R" , SaveType::NONE}, // NONE}            Road Rash - Jailbreak
            {"ACV" , SaveType::Flash_512k_SST}, // FLASH_V126      Robopon 2 - Cross Version
            {"ARP" , SaveType::Flash_512k_SST}, // FLASH_V126      Robopon 2 - Ring Version
            {"ARU" , SaveType::EEPROM_4k}, // EEPROM_V122     Robot Wars - Advanced Destruction
            {"ARW" , SaveType::EEPROM_4k}, // EEPROM_V122     Robot Wars - Advanced Destruction
            {"ARS" , SaveType::EEPROM_4k}, // EEPROM_V122     Robot Wars - Extreme Destruction
            {"ARB" , SaveType::EEPROM_4k}, // EEPROM_V122     Robotech - The Macross Saga
            {"BRT" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Robots
            {"BR7" , SaveType::NONE}, // NONE}            Rock 'Em Sock 'Em Robots
            {"A4R" , SaveType::EEPROM_4k}, // EEPROM_V122     Rock N' Roll Racing
            {"AR4" , SaveType::NONE}, // NONE}            Rocket Power - Beach Bandits
            {"ARK" , SaveType::NONE}, // NONE}            Rocket Power - Dream Scheme
            {"AZZ" , SaveType::NONE}, // NONE}            Rocket Power - Zero Gravity Zone
            {"AFC" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     RockMan & Forte
            {"BR4" , SaveType::Flash_512k_Panasonic_RTC}, // FLASH_V131      Rockman EXE 4.5 - Real Operation
            {"ARZ" , SaveType::SRAM_256K}, // SRAM_V112       RockMan Zero
            {"AR8" , SaveType::EEPROM_4k}, // EEPROM_V122     Rocky
            {"ARO" , SaveType::EEPROM_4k}, // EEPROM_V122     Rocky
            {"A8T" , SaveType::Flash_512k_Panasonic}, // FLASH_V130      RPG Tsukuru Advance
            {"BR3" , SaveType::NONE}, // NONE}            R-Type III
            {"ARG" , SaveType::NONE}, // NONE}            Rugrats - Castle Capers
            {"A5W" , SaveType::NONE}, // NONE}            Rugrats - Go Wild
            {"AR5" , SaveType::NONE}, // NONE}            Rugrats - I Gotta Go Party
            {"AWU" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Sabre Wulf
            {"A3B" , SaveType::NONE}, // NONE}            Sabrina - The Teenage Witch - Potion Commotion
            {"ASM" , SaveType::SRAM_256K}, // SRAM_V112       Saibara Rieko no Dendou Mahjong
            {"ACL" , SaveType::SRAM_256K}, // SRAM_V112       Sakura Momoko no UkiUki Carnival
            {"AS5" , SaveType::EEPROM_4k}, // EEPROM_V121     Salt Lake 2002
            {"AWG" , SaveType::EEPROM_4k}, // EEPROM_V121     Salt Lake 2002
            {"AOS" , SaveType::EEPROM_4k}, // EEPROM_V124     Samurai Deeper Kyo
            {"AEC" , SaveType::SRAM_256K}, // SRAM_V102       Samurai Evolution - Oukoku Geist
            {"AJT" , SaveType::EEPROM_4k}, // EEPROM_V122     Samurai Jack - The Amulet of Time
            {"ASX" , SaveType::Flash_512k_SST}, // FLASH_V126      San Goku Shi
            {"B3E" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      San Goku Shi Eiketsuden
            {"B3Q" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      San Goku Shi Koumeiden
            {"A85" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Sanrio Puroland All Characters
            {"ASN" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Sansara Naga 1x2
            {"AXX" , SaveType::EEPROM_4k}, // EEPROM_V122     Santa Claus Jr. Advance
            {"AUZ" , SaveType::NONE}, // NONE}            Santa Claus Saves the Earth
            {"A57" , SaveType::SRAM_256K}, // SRAM_V102       Scan Hunter - Sennen Kaigyo wo Oe!
            {"AP8" , SaveType::EEPROM_4k}, // EEPROM_V122     Scooby-Doo - The Motion Picture
            {"BMU" , SaveType::NONE}, // NONE}            Scooby-Doo 2 - Monsters Unleashed
            {"ASD" , SaveType::NONE}, // NONE}            Scooby-Doo and the Cyber Chase
            {"BMM" , SaveType::EEPROM_4k}, // EEPROM_V122     Scooby-Doo! - Mystery Mayhem
            {"B25" , SaveType::NONE}, // NONE}            Scooby-Doo! - Unmasked
            {"AQB" , SaveType::SRAM_256K}, // SRAM_V112       Scrabble
            {"BLA" , SaveType::NONE}, // NONE}            Scrabble Blast!
            {"BHV" , SaveType::EEPROM_4k}, // EEPROM_V124     Scurge - Hive
            {"BGE" , SaveType::EEPROM_4k}, // EEPROM_V124     SD Gundam Force
            {"BG4" , SaveType::EEPROM_4k}, // EEPROM_V124     SD Gundam Force
            {"BGA" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     SD Gundam G Generation
            {"ALJ" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Sea Trader - Rise of Taipan
            {"AAH" , SaveType::NONE}, // NONE}            Secret Agent Barbie - Royal Jewels Mission
            {"AYP" , SaveType::NONE}, // NONE}            Sega Arcade Gallery
            {"AYL" , SaveType::SRAM_256K}, // SRAM_V102       Sega Rally Championship
            {"A3P" , SaveType::NONE}, // NONE}            Sega Smash Pack
            {"A7G" , SaveType::SRAM_256K}, // SRAM_V112       Sengoku Kakumei Gaiden
            {"BKA" , SaveType::Flash_1M_Macronix}, // FLASH_V102      Sennen Kazoku
            {"BSY" , SaveType::SRAM_256K}, // SRAM_V113       Sentouin - Yamada Hajime
            {"AEN" , SaveType::NONE}, // NONE}            Serious Sam Advance
            {"BHL" , SaveType::SRAM_256K}, // SRAM_V103       Shaman King - Legacy of the Spirits - Soaring Hawk
            {"BWS" , SaveType::SRAM_256K}, // SRAM_V103       Shaman King - Legacy of the Spirits - Sprinting Wolf
            {"BSO" , SaveType::SRAM_256K}, // SRAM_V113       Shaman King - Master of Spirits
            {"AKA" , SaveType::SRAM_256K}, // SRAM_V112       Shaman King Card Game - Chou Senjiryakketsu 2
            {"AL3" , SaveType::SRAM_256K}, // SRAM_V112       Shaman King Card Game - Chou Senjiryakketsu 3
            {"BBA" , SaveType::EEPROM_4k_alt}, // EEPROM_V126     Shamu's Deep Sea Adventures
            {"BSH" , SaveType::EEPROM_4k}, // EEPROM_V122     Shanghai
            {"ASV" , SaveType::EEPROM_4k}, // EEPROM_V122     Shanghai Advance
            {"BSU" , SaveType::EEPROM_4k}, // EEPROM_V124     Shark Tale
            {"B9T" , SaveType::EEPROM_4k}, // EEPROM_V124     Shark Tale
            {"ASC" , SaveType::NONE}, // NONE}            Shaun Palmer's Pro Snowboarder
            {"AEP" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Sheep
            {"A6R" , SaveType::SRAM_256K}, // SRAM_V102       Shifting Gears - Road Trip
            {"B4K" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Shikakui Atama wo Marukusuru Advance - Kanji Keisan
            {"B4R" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Shikakui Atama wo Marukusuru Advance - Kokugo Sansu Rika Shakai
            {"A64" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Shimura Ken no Baka Tonosama
            {"U33" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Shin Bokura no Taiyou - Gyakushuu no Sabata
            {"AAU" , SaveType::SRAM_256K}, // SRAM_V112       Shin Megami Tensei
            {"BDL" , SaveType::SRAM_256K}, // SRAM_V113       Shin Megami Tensei - Devil Children Messiah Riser
            {"A5T" , SaveType::SRAM_256K}, // SRAM_V113       Shin Megami Tensei 2
            {"BDH" , SaveType::SRAM_256K}, // SRAM_V103       Shin Megami Tensei Devil Children - Honoo no Sho
            {"BDY" , SaveType::SRAM_256K}, // SRAM_V103       Shin Megami Tensei Devil Children - Koori no Sho
            {"A8Z" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Shin Megami Tensei Devil Children - Puzzle de Call!
            {"ARA" , SaveType::SRAM_256K}, // SRAM_V112       Shin Nippon Pro Wrestling Toukon Retsuden Advance
            {"BKV" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Shingata Medarot - Kabuto Version
            {"BKU" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Shingata Medarot - Kuwagata Version
            {"AF5" , SaveType::SRAM_256K}, // SRAM_V113       Shining Force - Resurrection of the Dark Dragon
            {"AHU" , SaveType::Flash_512k_Panasonic}, // FLASH_V130      Shining Soul
            {"AU2" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Shining Soul II
            {"ANV" , SaveType::SRAM_256K}, // SRAM_V102       Shiren Monsters - Netsal
            {"B2M" , SaveType::SRAM_256K}, // SRAM_V113       Shonen Jump's Shaman King - Master of Spirits 2
            {"AH4" , SaveType::EEPROM_4k}, // EEPROM_V122     Shrek - Hassle at the Castle
            {"AOI" , SaveType::EEPROM_4k}, // EEPROM_V122     Shrek - Reekin' Havoc
            {"B4I" , SaveType::EEPROM_4k}, // EEPROM_V124     Shrek - Smash n' Crash Racing
            {"B4U" , SaveType::EEPROM_4k}, // EEPROM_V124     Shrek - Super Slam
            {"AS4" , SaveType::EEPROM_4k}, // EEPROM_V122     Shrek - Swamp Kart Speedway
            {"BSE" , SaveType::EEPROM_4k}, // EEPROM_V124     Shrek 2
            {"BSI" , SaveType::EEPROM_4k}, // EEPROM_V124     Shrek 2 - Beg for Mercy
            {"B3H" , SaveType::EEPROM_4k}, // EEPROM_V124     Shrek the Third
            {"B3G" , SaveType::EEPROM_4k}, // EEPROM_V124     Sigma Star Saga
            {"AIP" , SaveType::EEPROM_4k}, // EEPROM_V122     Silent Scope
            {"A7I" , SaveType::SRAM_256K}, // SRAM_V112       Silk to Cotton
            {"A5C" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Sim City 2000
            {"AZK" , SaveType::EEPROM_4k}, // EEPROM_V122     Simple 2960 Tomodachi Series Vol. 1 - The Table Game Collection
            {"AZ9" , SaveType::EEPROM_4k}, // EEPROM_V122     Simple 2960 Tomodachi Series Vol. 2 - The Block Kuzushi
            {"BS3" , SaveType::EEPROM_4k}, // EEPROM_V124     Simple 2960 Tomodachi Series Vol. 3 - The Itsudemo Puzzle
            {"BS4" , SaveType::EEPROM_4k}, // EEPROM_V124     Simple 2960 Tomodachi Series Vol. 4 - The Trump
            {"AAJ" , SaveType::SRAM_256K}, // SRAM_V113       Sin Kisekae Monogatari
            {"A4P" , SaveType::EEPROM_4k}, // EEPROM_V122     Sister Princess - RePure
            {"BSD" , SaveType::NONE}, // NONE}            Sitting Ducks
            {"B4D" , SaveType::NONE}, // NONE}            Sky Dancers - They Magically Fly!
            {"A9K" , SaveType::EEPROM_4k}, // EEPROM_V122     Slime Morimori Dragon Quest - Shougeki no Shippo Dan
            {"ATB" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Slot! Pro 2 Advance - GoGo Juggler & New Tairyou
            {"ASF" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Slot! Pro Advance - Takarabune & Ooedo Sakurafubuki 2
            {"BSV" , SaveType::NONE}, // NONE}            Smashing Drive
            {"ASG" , SaveType::NONE}, // NONE}            Smuggler's Run
            {"AEA" , SaveType::SRAM_256K}, // SRAM_V102       Snap Kid's
            {"ASQ" , SaveType::NONE}, // NONE}            Snood
            {"B2V" , SaveType::NONE}, // NONE}            Snood 2 - On Vacation
            {"AK6" , SaveType::NONE}, // NONE}            Soccer Kid
            {"ALS" , SaveType::EEPROM_4k}, // EEPROM_V122     Soccer Mania
            {"ASO" , SaveType::Flash_512k_SST}, // FLASH_V126      Sonic Advance
            {"A2N" , SaveType::Flash_512k_Panasonic}, // FLASH_V130      Sonic Advance 2
            {"B3S" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Sonic Advance 3
            {"BSB" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Sonic Battle
            {"A3V" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Sonic Pinball Party
            {"A86" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Sonic Pinball Party
            {"BIJ" , SaveType::EEPROM_4k}, // EEPROM_V124     Sonic The Hedgehog - Genesis
            {"B67" , SaveType::NONE}, // NONE}            Sorry! - Aggravation - Scrabble Junior
            {"A5U" , SaveType::EEPROM_4k}, // EEPROM_V122     Space Channel 5 - Ulala's Cosmic Attack
            {"AJS" , SaveType::SRAM_256K}, // SRAM_V111       Space Hexcite - Maetel Legend EX
            {"AID" , SaveType::EEPROM_4k}, // EEPROM_V120     Space Invaders
            {"AS6" , SaveType::EEPROM_4k}, // EEPROM_V120     Speedball 2 - Brutal Deluxe
            {"AKX" , SaveType::EEPROM_4k}, // EEPROM_V122     Spider-Man
            {"BC9" , SaveType::EEPROM_4k}, // EEPROM_V124     Spider-Man - Battle For New York
            {"ASE" , SaveType::NONE}, // NONE}            Spider-Man - Mysterio's Menace
            {"BSP" , SaveType::EEPROM_4k}, // EEPROM_V124     Spider-Man 2
            {"BI3" , SaveType::EEPROM_4k}, // EEPROM_V124     Spider-Man 3
            {"AC6" , SaveType::NONE}, // NONE}            Spirit - Stallion of the Cimarron
            {"AWN" , SaveType::NONE}, // NONE}            Spirit & Spells
            {"BSQ" , SaveType::NONE}, // NONE}            SpongeBob SquarePants - Battle for Bikini Bottom
            {"BQ4" , SaveType::EEPROM_4k}, // EEPROM_V124     SpongeBob SquarePants - Creature from the Krusty Krab
            {"BQQ" , SaveType::EEPROM_4k}, // EEPROM_V124     SpongeBob SquarePants - Lights, Camera, Pants!
            {"AQ3" , SaveType::NONE}, // NONE}            SpongeBob SquarePants - Revenge of the Flying Dutchman
            {"ASP" , SaveType::NONE}, // NONE}            SpongeBob SquarePants - SuperSponge
            {"BZX" , SaveType::NONE}, // NONE}            SpongeBob's Atlantis Squarepantis
            {"AKB" , SaveType::SRAM_256K}, // SRAM_V112       Sports Illustrated for Kids - Baseball
            {"AKF" , SaveType::SRAM_256K}, // SRAM_V112       Sports Illustrated for Kids - Football
            {"B23" , SaveType::EEPROM_4k}, // EEPROM_V124     Sportsman's Pack - Cabela's Big Game Hunter + Rapala Pro Fishing
            {"B6A" , SaveType::NONE}, // NONE}            Spy Hunter - Super Sprint
            {"AV3" , SaveType::EEPROM_4k}, // EEPROM_V122     Spy Kids 3-D - Game Over
            {"A2K" , SaveType::EEPROM_4k}, // EEPROM_V120     Spy Kids Challenger
            {"BSS" , SaveType::NONE}, // NONE}            Spy Muppets - License to Croak
            {"AHN" , SaveType::EEPROM_4k}, // EEPROM_V122     SpyHunter
            {"AOW" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Spyro - Attack of the Rhynocs
            {"ASY" , SaveType::EEPROM_4k}, // EEPROM_V122     Spyro - Season of Ice
            {"A2S" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Spyro 2 - Season of Flame
            {"A4S" , SaveType::EEPROM_4k}, // EEPROM_V122     Spyro Advance
            {"BS8" , SaveType::EEPROM_4k}, // EEPROM_V124     Spyro Advance - Wakuwaku Tomodachi
            {"BST" , SaveType::EEPROM_4k}, // EEPROM_V124     Spyro Orange - The Cortex Conspiracy
            {"B8S" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Spyro Superpack - Season of Ice + Season of Flame
            {"BSX" , SaveType::EEPROM_4k}, // EEPROM_V122     SSX 3
            {"AXY" , SaveType::EEPROM_4k}, // EEPROM_V120     SSX Tricky
            {"A9G" , SaveType::NONE}, // NONE}            Stadium Games
            {"BSW" , SaveType::NONE}, // NONE}            Star Wars - Flight of the Falcon
            {"ASW" , SaveType::NONE}, // NONE}            Star Wars - Jedi Power Battles
            {"A2W" , SaveType::NONE}, // NONE}            Star Wars - The New Droid Army
            {"AS2" , SaveType::NONE}, // NONE}            Star Wars Episode II - Attack of the Clones
            {"BE3" , SaveType::EEPROM_4k}, // EEPROM_V124     Star Wars Episode III - Revenge of the Sith
            {"BCK" , SaveType::EEPROM_4k}, // EEPROM_V124     Star Wars Trilogy - Apprentice of the Force
            {"AS8" , SaveType::NONE}, // NONE}            Star X
            {"AYH" , SaveType::EEPROM_4k}, // EEPROM_V122     Starsky & Hutch
            {"BKT" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Steel Empire
            {"ATU" , SaveType::EEPROM_4k}, // EEPROM_V122     Steven Gerrard's Total Soccer 2002
            {"B35" , SaveType::NONE}, // NONE}            Strawberry Shortcake - Summertime Adventure
            {"BQW" , SaveType::NONE}, // NONE}            Strawberry Shortcake - Summertime Adventure - Special Edition
            {"B4T" , SaveType::EEPROM_4k}, // EEPROM_V124     Strawberry Shortcake - Sweet Dreams
            {"AZU" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Street Fighter Alpha 3 - Upper
            {"A3Z" , SaveType::EEPROM_4k}, // EEPROM_V122     Street Jam Basketball
            {"BCZ" , SaveType::EEPROM_4k}, // EEPROM_V124     Street Racing Syndicate
            {"AFH" , SaveType::NONE}, // NONE}            Strike Force Hydra
            {"ASL" , SaveType::NONE}, // NONE}            Stuart Little 2
            {"AUX" , SaveType::EEPROM_4k}, // EEPROM_V122     Stuntman
            {"B4L" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Sugar Sugar Une - Heart Gaippai! Moegi Gakuen
            {"AB4" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Summon Night - Swordcraft Story
            {"BSK" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Summon Night - Swordcraft Story 2
            {"B3C" , SaveType::EEPROM_8k}, // EEPROM_V126     Summon Night Craft Sword Monogatari - Hajimari no Ishi
            {"BG6" , SaveType::NONE}, // NONE}            Super Army War
            {"AVZ" , SaveType::EEPROM_4k}, // EEPROM_V122     Super Bubble Pop
            {"ABM" , SaveType::NONE}, // NONE}            Super Bust-A-Move
            {"BSA" , SaveType::EEPROM_4k}, // EEPROM_V124     Super Chinese 1 - 2 Advance
            {"BCL" , SaveType::NONE}, // NONE}            Super Collapse! II
            {"ADF" , SaveType::SRAM_256K}, // SRAM_V110       Super Dodge Ball Advance
            {"BDP" , SaveType::EEPROM_4k}, // EEPROM_V122     Super Duper Sumos
            {"AG5" , SaveType::EEPROM_4k}, // EEPROM_V122     Super Ghouls 'N Ghosts
            {"BF8" , SaveType::NONE}, // NONE}            Super Hornet FA 18F
            {"AMA" , SaveType::EEPROM_4k}, // EEPROM_V120     Super Mario Advance
            {"AMZ" , SaveType::EEPROM_4k}, // EEPROM_V120     Super Mario Advance (Kiosk Demo)
            {"AX4" , SaveType::Flash_1M_Macronix}, // FLASH_V102      Super Mario Advance 4 - Super Mario Bros. 3
            {"AA2" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Super Mario World - Super Mario Advance 2
            {"ALU" , SaveType::EEPROM_4k}, // EEPROM_V122     Super Monkey Ball Jr.
            {"AZ8" , SaveType::EEPROM_4k}, // EEPROM_V122     Super Puzzle Fighter II Turbo
            {"BDM" , SaveType::EEPROM_4k}, // EEPROM_V124     Super Real Mahjong Dousoukai
            {"AOG" , SaveType::Flash_512k_Panasonic}, // FLASH_V130      Super Robot Taisen - Original Generation
            {"B2R" , SaveType::Flash_512k_Panasonic}, // FLASH_V130      Super Robot Taisen - Original Generation 2
            {"ASR" , SaveType::Flash_512k_SST}, // FLASH_V125      Super Robot Taisen A
            {"A6S" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Super Robot Taisen D
            {"B6J" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Super Robot Taisen J
            {"AJ9" , SaveType::Flash_512k_SST}, // FLASH_V126      Super Robot Taisen R
            {"AXR" , SaveType::EEPROM_4k}, // EEPROM_V121     Super Street Fighter II Turbo - Revival
            {"ASU" , SaveType::NONE}, // NONE}            Superman - Countdown to Apokolips
            {"BQX" , SaveType::EEPROM_4k}, // EEPROM_V124     Superman Returns - Fortress of Solitude
            {"BXU" , SaveType::EEPROM_4k}, // EEPROM_V124     Surf's Up
            {"ASK" , SaveType::SRAM_256K}, // SRAM_V110       Sutakomi - Star Communicator
            {"ABG" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Sweet Cookie Pie
            {"AVS" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Sword of Mana
            {"BSF" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Sylvania Family - Fashion Designer ni Naritai
            {"A4L" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Sylvania Family 4 - Meguru Kisetsu no Tapestry
            {"BS5" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Sylvanian Family - Yousei no Stick to Fushigi no Ki
            {"ATO" , SaveType::Flash_512k_SST}, // FLASH_V126      Tactics Ogre - The Knight of Lodis
            {"BU6" , SaveType::EEPROM_4k}, // EEPROM_V124     Taiketsu! Ultra Hero
            {"BJW" , SaveType::EEPROM_4k}, // EEPROM_V124     Tak - The Great Juju Challenge
            {"BT9" , SaveType::EEPROM_4k}, // EEPROM_V124     Tak 2 - The Staff of Dreams
            {"BJU" , SaveType::EEPROM_4k}, // EEPROM_V122     Tak and the Power of Juju
            {"AN8" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Tales of Phantasia
            {"AN9" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Tales of the World - Narikiri Dungeon 2
            {"B3T" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Tales of the World - Narikiri Dungeon 3
            {"A9P" , SaveType::SRAM_256K}, // SRAM_V113       Tales of the World - Summoner's Lineage
            {"AYM" , SaveType::SRAM_256K}, // SRAM_V100       Tanbi Musou - Meine Liebe
            {"ATA" , SaveType::EEPROM_4k}, // EEPROM_V121     Tang Tang
            {"BTI" , SaveType::SRAM_256K}, // SRAM_V113       Tantei Gakuen Q - Kyukyoku Trick ni Idome!
            {"BTQ" , SaveType::SRAM_256K}, // SRAM_V113       Tantei Gakuen Q - Meitantei ha Kimi da!
            {"BT3" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Tantei Jinguuji Saburou Shiroi Kage no Syoujyo
            {"AJG" , SaveType::EEPROM_4k}, // EEPROM_V122     Tarzan - Return to the Jungle
            {"AXQ" , SaveType::NONE}, // NONE}            Taxi 3
            {"BBL" , SaveType::EEPROM_4k}, // EEPROM_V124     Teen Titans
            {"BZU" , SaveType::EEPROM_4k}, // EEPROM_V124     Teen Titans 2
            {"BNT" , SaveType::EEPROM_4k}, // EEPROM_V122     Teenage Mutant Ninja Turtles
            {"BT2" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Teenage Mutant Ninja Turtles 2 - Battlenexus
            {"BT8" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Teenage Mutant Ninja Turtles Double Pack
            {"ATK" , SaveType::EEPROM_4k}, // EEPROM_V122     Tekken Advance
            {"BTP" , SaveType::NONE}, // NONE}            Ten Pin Alley 2
            {"AT8" , SaveType::EEPROM_4k}, // EEPROM_V120     Tennis Masters Series 2003
            {"AVA" , SaveType::SRAM_256K}, // SRAM_V112       Tennis no Ouji-sama - Aim at the Victory!
            {"ATI" , SaveType::Flash_512k_SST}, // FLASH_V126      Tennis no Ouji-sama - Genius Boys Academy
            {"A9L" , SaveType::SRAM_256K}, // SRAM_V102       Tennis no Ouji-sama 2003 - Cool Blue
            {"A8R" , SaveType::SRAM_256K}, // SRAM_V102       Tennis no Ouji-sama 2003 - Passion Red
            {"B4G" , SaveType::SRAM_256K}, // SRAM_V103       Tennis no Ouji-sama 2004 - Glorious Gold
            {"B4S" , SaveType::SRAM_256K}, // SRAM_V103       Tennis no Ouji-sama 2004 - Stylish Silver
            {"AO3" , SaveType::NONE}, // NONE}            Terminator 3 - Rise of the Machines
            {"BTT" , SaveType::EEPROM_4k}, // EEPROM_V124     Tetris Advance
            {"ATW" , SaveType::NONE}, // NONE}            Tetris Worlds
            {"BXA" , SaveType::NONE}, // NONE}            Texas Hold 'Em Poker
            {"BRV" , SaveType::EEPROM_4k}, // EEPROM_V124     That's So Raven
            {"BZS" , SaveType::EEPROM_4k}, // EEPROM_V124     Thats So Raven 2 - Supernatural Style
            {"BJN" , SaveType::NONE}, // NONE}            The Adventures of Jimmy Neutron - Boy Genius - Jet Fusion
            {"AJX" , SaveType::NONE}, // NONE}            The Adventures of Jimmy Neutron vs Jimmy Negatron
            {"A7M" , SaveType::NONE}, // NONE}            The Amazing Virtual Sea Monkeys
            {"BUY" , SaveType::EEPROM_4k}, // EEPROM_V124     The Ant Bully
            {"BBI" , SaveType::EEPROM_4k}, // EEPROM_V124     The Barbie Diaries - High School Mystery
            {"BKF" , SaveType::EEPROM_4k}, // EEPROM_V124     The Bee Game
            {"BBO" , SaveType::NONE}, // NONE}            The Berenstain Bears - And the Spooky Old Tree
            {"BCT" , SaveType::NONE}, // NONE}            The Cat in the Hat by Dr. Seuss
            {"BCQ" , SaveType::EEPROM_4k}, // EEPROM_V124     The Cheetah Girls
            {"B2W" , SaveType::EEPROM_4k}, // EEPROM_V124     The Chronicles of Narnia - The Lion, the Witch and the Wardrobe
            {"AF6" , SaveType::NONE}, // NONE}            The Fairly Odd Parents! - Breakin' da Rules
            {"BFO" , SaveType::NONE}, // NONE}            The Fairly Odd Parents! - Clash with the Anti-World
            {"AFV" , SaveType::NONE}, // NONE}            The Fairly Odd Parents! - Enter the Cleft
            {"BF2" , SaveType::NONE}, // NONE}            The Fairly Odd Parents! - Shadow Showdown
            {"AFS" , SaveType::NONE}, // NONE}            The Flintstones - Big Trouble in Bedrock
            {"BIE" , SaveType::EEPROM_4k}, // EEPROM_V124     The Grim - Adventures of Billy and Mandy
            {"ALI" , SaveType::NONE}, // NONE}            The Haunted Mansion
            {"AH9" , SaveType::EEPROM_4k}, // EEPROM_V122     The Hobbit
            {"AHL" , SaveType::EEPROM_4k}, // EEPROM_V122     The Incredible Hulk
            {"BIC" , SaveType::NONE}, // NONE}            The Incredibles
            {"BIQ" , SaveType::NONE}, // NONE}            The Incredibles - Rise of the Underminer
            {"AIO" , SaveType::EEPROM_4k}, // EEPROM_V120     The Invincible Iron Man
            {"AJF" , SaveType::NONE}, // NONE}            The Jungle Book
            {"AKO" , SaveType::EEPROM_4k}, // EEPROM_V122     The King of Fighters EX - Neo Blood
            {"AEX" , SaveType::EEPROM_4k}, // EEPROM_V122     The King of Fighters EX2 - Howling Blood
            {"BAK" , SaveType::NONE}, // NONE}            The Koala Brothers - Outback Adventures
            {"ALA" , SaveType::NONE}, // NONE}            The Land Before Time
            {"BLO" , SaveType::EEPROM_4k}, // EEPROM_V124     The Land Before Time - Into the Mysterious Beyond
            {"B3Y" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     The Legend of Spyro - A New Beginning
            {"BU7" , SaveType::EEPROM_4k_alt}, // EEPROM_V126     The Legend of Spyro - The Eternal Night
            {"AZL" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     The Legend of Zelda - A Link to the Past & Four Swords
            {"BZM" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     The Legend of Zelda - The Minish Cap
            {"BLK" , SaveType::EEPROM_4k}, // EEPROM_V122     The Lion King 1.5
            {"BN9" , SaveType::EEPROM_4k}, // EEPROM_V124     The Little Mermaid - Magic in Two Kingdoms
            {"ALO" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     The Lord of the Rings - The Fellowship of the Ring
            {"BLR" , SaveType::EEPROM_4k}, // EEPROM_V122     The Lord of the Rings - The Return of the King
            {"B3A" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     The Lord of the Rings - The Third Age
            {"ALP" , SaveType::EEPROM_4k}, // EEPROM_V122     The Lord of the Rings - The Two Towers
            {"ALV" , SaveType::EEPROM_4k}, // EEPROM_V122     The Lost Vikings
            {"AUM" , SaveType::NONE}, // NONE}            The Mummy
            {"AZM" , SaveType::NONE}, // NONE}            The Muppets - On With the Show!
            {"APD" , SaveType::EEPROM_4k}, // EEPROM_V122     The Pinball of the Dead
            {"AZO" , SaveType::EEPROM_4k}, // EEPROM_V122     The Pinball of the Dead
            {"BPX" , SaveType::EEPROM_4k}, // EEPROM_V124     The Polar Express
            {"AP5" , SaveType::EEPROM_4k}, // EEPROM_V122     The Powerpuff Girls - Him and Seek
            {"APT" , SaveType::EEPROM_4k}, // EEPROM_V122     The Powerpuff Girls - Mojo Jojo A-Go-Go
            {"BD7" , SaveType::EEPROM_4k}, // EEPROM_V124     The Proud Family
            {"A3R" , SaveType::NONE}, // NONE}            The Revenge of Shinobi
            {"ARD" , SaveType::NONE}, // NONE}            The Ripping Friends
            {"B33" , SaveType::EEPROM_4k}, // EEPROM_V124     The Santa Clause 3 - The Escape Clause
            {"ASZ" , SaveType::NONE}, // NONE}            The Scorpion King - Sword of Osiris
            {"A4A" , SaveType::NONE}, // NONE}            The Simpsons - Road Rage
            {"B4P" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     The Sims
            {"ASI" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     The Sims - Bustin' Out
            {"B46" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      The Sims 2
            {"B4O" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      The Sims 2 - Pets
            {"A7S" , SaveType::NONE}, // NONE}            The Smurfs - The Revenge of the Smurfs
            {"BSN" , SaveType::NONE}, // NONE}            The SpongeBob SquarePants Movie
            {"BZC" , SaveType::EEPROM_4k}, // EEPROM_V124     The Suite Life of Zack & Cody - Tipton Trouble
            {"AA6" , SaveType::EEPROM_4k}, // EEPROM_V122     The Sum of All Fears
            {"A3T" , SaveType::NONE}, // NONE}            The Three Stooges
            {"BTR" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      The Tower SP
            {"BOC" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      The Urbz - Sims in the City
            {"BWL" , SaveType::EEPROM_4k}, // EEPROM_V124     The Wild
            {"AWT" , SaveType::NONE}, // NONE}            The Wild Thornberrys - Chimp Chase
            {"AWL" , SaveType::NONE}, // NONE}            The Wild Thornberrys Movie
            {"BTH" , SaveType::NONE}, // NONE}            Thunder Alley
            {"BTB" , SaveType::NONE}, // NONE}            Thunderbirds
            {"ATN" , SaveType::NONE}, // NONE}            Thunderbirds - International Rescue
            {"BTW" , SaveType::EEPROM_4k}, // EEPROM_V122     Tiger Woods PGA Tour 2004
            {"AT5" , SaveType::EEPROM_4k}, // EEPROM_V122     Tiger Woods PGA Tour Golf
            {"BNC" , SaveType::EEPROM_4k}, // EEPROM_V124     Tim Burton's The Nightmare Before Christmas - The Pumpkin King
            {"ATT" , SaveType::NONE}, // NONE}            Tiny Toon Adventures - Scary Dreams
            {"AWS" , SaveType::NONE}, // NONE}            Tiny Toon Adventures - Wacky Stackers
            {"ATV" , SaveType::NONE}, // NONE}            Tir et But - Edition Champions du Monde
            {"BTC" , SaveType::NONE}, // NONE}            Titeuf - Mega-Compet
            {"BEX" , SaveType::EEPROM_4k}, // EEPROM_V124     TMNT
            {"ATQ" , SaveType::EEPROM_4k}, // EEPROM_V122     TOCA World Touring Cars
            {"AF7" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Tokimeki Yume Series 1 - Ohanaya-san ni Narou!
            {"BTF" , SaveType::SRAM_256K}, // SRAM_V113       Tokyo Majin Gakuen - Fuju Houroku
            {"BTZ" , SaveType::EEPROM_4k}, // EEPROM_V124     Tokyo Xtreme Racer Advance
            {"ATJ" , SaveType::NONE}, // NONE}            Tom and Jerry - The Magic Ring
            {"AIF" , SaveType::NONE}, // NONE}            Tom and Jerry in Infurnal Escape
            {"BJT" , SaveType::NONE}, // NONE}            Tom and Jerry Tales
            {"AR6" , SaveType::EEPROM_4k}, // EEPROM_V122     Tom Clancy's Rainbow Six - Rogue Spear
            {"AO4" , SaveType::EEPROM_4k}, // EEPROM_V122     Tom Clancy's Splinter Cell
            {"BSL" , SaveType::EEPROM_4k}, // EEPROM_V124     Tom Clancy's Splinter Cell - Pandora Tomorrow
            {"AGL" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Tomato Adventure
            {"BL8" , SaveType::EEPROM_4k}, // EEPROM_V126     Tomb Raider - Legend
            {"AL9" , SaveType::NONE}, // NONE}            Tomb Raider - The Prophecy
            {"AUT" , SaveType::NONE}, // NONE}            Tomb Raider - The Prophecy
            {"BT7" , SaveType::NONE}, // NONE}            Tonka - On the Job
            {"BH9" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Tony Hawk's American Sk8land
            {"BXS" , SaveType::EEPROM_4k}, // EEPROM_V124     Tony Hawk's Downhill Jam
            {"ATH" , SaveType::EEPROM_4k}, // EEPROM_V111     Tony Hawk's Pro Skater 2
            {"AT3" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Tony Hawk's Pro Skater 3
            {"AT6" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Tony Hawk's Pro Skater 4
            {"BTO" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Tony Hawk's Underground
            {"B2T" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Tony Hawk's Underground 2
            {"AT7" , SaveType::NONE}, // NONE}            Tootuff - The Gagmachine
            {"A2Y" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Top Gun - Combat Zones
            {"ATG" , SaveType::NONE}, // NONE}            Top Gun - Firestorm Advance
            {"B27" , SaveType::EEPROM_4k}, // EEPROM_V124     Top Spin 2
            {"ATC" , SaveType::EEPROM_4k}, // EEPROM_V120     TopGear GT Championship
            {"BTG" , SaveType::EEPROM_4k}, // EEPROM_V122     TopGear Rally
            {"AYE" , SaveType::EEPROM_4k}, // EEPROM_V122     TopGear Rally
            {"BTU" , SaveType::EEPROM_4k}, // EEPROM_V124     Totally Spies
            {"B2L" , SaveType::EEPROM_4k}, // EEPROM_V124     Totally Spies 2 - Undercover
            {"A59" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Toukon Heat
            {"ATR" , SaveType::SRAM_256K}, // SRAM_V112       Toyrobo Force
            {"AZQ" , SaveType::NONE}, // NONE}            Treasure Planet
            {"B9S" , SaveType::EEPROM_4k}, // EEPROM_V124     Trick Star
            {"BTJ" , SaveType::EEPROM_4k}, // EEPROM_V124     Tringo
            {"BT6" , SaveType::EEPROM_4k}, // EEPROM_V124     Trollz - Hair Affair!
            {"BTN" , SaveType::EEPROM_4k}, // EEPROM_V124     Tron 2.0 - Killer App
            {"AK3" , SaveType::EEPROM_4k}, // EEPROM_V122     Turbo Turtle Adventure
            {"AT4" , SaveType::NONE}, // NONE}            Turok Evolution
            {"ATM" , SaveType::SRAM_256K}, // SRAM_V112       Tweety and the Magic Gems
            {"AMJ" , SaveType::SRAM_256K}, // SRAM_V110       Tweety no Hearty Party
            {"BFV" , SaveType::EEPROM_4k}, // EEPROM_V124     Twin Series 1 - Fashion Designer Monogatari + Kawaii Pet Game Gallery 2
            {"BOP" , SaveType::EEPROM_4k}, // EEPROM_V124     Twin Series 2 - Oshare Princess 4 + Renai Uranai Daisakusen
            {"BQM" , SaveType::EEPROM_4k}, // EEPROM_V124     Twin Series 3 - Konchuu Monster + Suchai Labyrinth
            {"BHF" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Twin Series 4 - Ham Ham Monster EX + Fantasy Puzzle Hamster Monogatari
            {"BMW" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Twin Series 5 - Wan Wan Meitantei EX + Mahou no Kuni no Keaki-Okusan Monogatari
            {"BWN" , SaveType::EEPROM_4k}, // EEPROM_V124     Twin Series 6 - Wan Nyon Idol Gakuen + Koinu Toissho Special
            {"B2P" , SaveType::EEPROM_4k}, // EEPROM_V124     Twin Series 7 - Twin Puzzle - Kisekae Wanko Ex + Puzzle Rainbow Magic 2
            {"BTY" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Ty the Tasmanian Tiger 2 - Bush Rescue
            {"BTV" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Ty the Tasmanian Tiger 3 - Night of the Quinkan
            {"BUV" , SaveType::SRAM_256K}, // SRAM_V113       Uchu no Stellvia
            {"AUC" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Uchuu Daisakusen Choco Vader - Uchuu Kara no Shinryakusha
            {"BUH" , SaveType::EEPROM_4k}, // EEPROM_V124     Ueki no Housoku Shinki Sakuretsu! Nouryokumono Battle
            {"AEW" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Ui-Ire - World Soccer Winning Eleven
            {"BUZ" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Ultimate Arcade Games
            {"AVE" , SaveType::EEPROM_4k}, // EEPROM_V122     Ultimate Beach Soccer
            {"ABU" , SaveType::EEPROM_4k}, // EEPROM_V122     Ultimate Brain Games
            {"BUC" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Ultimate Card Games
            {"AK2" , SaveType::EEPROM_4k}, // EEPROM_V122     Ultimate Muscle - The Kinnikuman Legacy - The Path of the Superhero
            {"BUA" , SaveType::EEPROM_4k}, // EEPROM_V124     Ultimate Puzzle Games
            {"BUL" , SaveType::EEPROM_4k}, // EEPROM_V124     Ultimate Spider-Man
            {"BUW" , SaveType::NONE}, // NONE}            Ultimate Winter Games
            {"BUT" , SaveType::Flash_512k_Panasonic}, // FLASH_V131      Ultra Keibitai - Monster Attack
            {"BU4" , SaveType::NONE}, // NONE}            Unfabulous
            {"BU5" , SaveType::NONE}, // NONE}            Uno 52
            {"BUI" , SaveType::NONE}, // NONE}            Uno Freefall
            {"AYI" , SaveType::NONE}, // NONE}            Urban Yeti!
            {"AVP" , SaveType::NONE}, // NONE}            V.I.P.
            {"BAN" , SaveType::NONE}, // NONE}            Van Helsing
            {"BRX" , SaveType::SRAM_256K}, // SRAM_V103       Vattroller X
            {"BZT" , SaveType::EEPROM_4k}, // EEPROM_V124     VeggieTales - LarryBoy and the Bad Apple
            {"AVT" , SaveType::EEPROM_4k}, // EEPROM_V121     Virtua Tennis
            {"AVK" , SaveType::EEPROM_4k}, // EEPROM_V121     Virtual Kasparov
            {"AVM" , SaveType::EEPROM_4k}, // EEPROM_V122     V-Master Cross
            {"AVR", SaveType::EEPROM_4k}, // EEPROM_V122     V-Rally 3
                { "BWT" , SaveType::EEPROM_4k}, // EEPROM_V124     W.i.t.c.h.
                {"BSR" , SaveType::EEPROM_4k}, // EEPROM_V122     Wade Hixton's Counter Punch
                {"BMI" , SaveType::SRAM_256K}, // SRAM_V113       Wagamama - Fairy Milmo de Pon! DokiDoki Memorial
                {"BWP" , SaveType::SRAM_256K}, // SRAM_V113       Wagamama Fairy Mirumo de Pon Nazo no Kagi to Shinjitsu no Tobir
                {"BMY" , SaveType::SRAM_256K}, // SRAM_V103       Wagamama Fairy Mirumo de Pon! - 8 Nin no Toki no Yousei
                {"AWK" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Wagamama Fairy Mirumo de Pon! - Ougon Maracas no Densetsu
                {"BMP" , SaveType::SRAM_256K}, // SRAM_V103       Wagamama Fairy Mirumo de Pon! - Taisen Mahoudama
                {"BWF" , SaveType::SRAM_256K}, // SRAM_V113       Wagamama Fairy Mirumo de Pon! Yume no Kakera
                {"AWD" , SaveType::EEPROM_4k}, // EEPROM_V120     Wakeboarding Unleashed featuring Shaun Murray
                {"BWD" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Wan Nyan Doubutsu Byouin
                {"BWK" , SaveType::EEPROM_4k}, // EEPROM_V124     Wanko Dekururi! Wankuru
                {"BWX" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Wanko Mix Chiwanko World
                {"BWM" , SaveType::EEPROM_4k}, // EEPROM_V124     WanWan Meitantei
                {"AWA" , SaveType::SRAM_256K}, // SRAM_V112       Wario Land 4
                {"RZW" , SaveType::SRAM_256K}, // SRAM_V113       WarioWare Twisted!
                {"AZW" , SaveType::SRAM_256K}, // SRAM_V112       WarioWare, Inc. - Mega Microgames!
                {"AW3" , SaveType::EEPROM_4k}, // EEPROM_V122     Watashi no Makesalon
                {"BWE" , SaveType::EEPROM_4k}, // EEPROM_V124     Whac-A-Mole
                {"A73" , SaveType::SRAM_256K}, // SRAM_V113       Whistle! - Dai 37 Kai Tokyo-to Chuugakkou Sougou Taiiku Soccer Taikai
                {"A55" , SaveType::NONE}, // NONE}            Who Wants to Be a Millionaire
                {"B55" , SaveType::NONE}, // NONE}            Who Wants to Be a Millionaire - 2nd Edition
                {"BWJ" , SaveType::NONE}, // NONE}            Who Wants to Be a Millionaire - Junior
                {"AW9" , SaveType::EEPROM_4k}, // EEPROM_V122     Wing Commander - Prophecy
                {"AWQ" , SaveType::EEPROM_4k}, // EEPROM_V122     Wings
                {"BWH" , SaveType::NONE}, // NONE}            Winnie the Pooh's Rumbly Tumbly Adventure
                {"BWZ" , SaveType::EEPROM_4k}, // EEPROM_V124     Winnie the Pooh's Rumbly Tumbly Adventure + Rayman 3
                {"AWP" , SaveType::Flash_512k_Atmel}, // FLASH_V121      Winning Post for GameBoy Advance
                {"BWY" , SaveType::EEPROM_4k}, // EEPROM_V124     Winter Sports
                {"BWI" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     WinX Club
                {"BWV" , SaveType::EEPROM_4k}, // EEPROM_V124     Winx Club - Quest For The Codex
                {"AWZ" , SaveType::SRAM_256K}, // SRAM_V102       Wizardry Summoner
                {"AWO" , SaveType::EEPROM_4k}, // EEPROM_V120     Wolfenstein 3D
                {"AWW" , SaveType::EEPROM_4k}, // EEPROM_V122     Woody Woodpecker in Crazy Castle 5
                {"BB8" , SaveType::EEPROM_4k}, // EEPROM_V124     Word Safari - The Friendship Totems
                {"AAS" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     World Advance Soccer - Shouri heno Michi
                {"BP9" , SaveType::NONE}, // NONE}            World Championship Poker
                {"B26" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     World Poker Tour
                {"BWO" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     World Poker Tour
                {"BWR" , SaveType::NONE}, // NONE}            World Reborn
                {"AWC" , SaveType::NONE}, // NONE}            World Tennis Stars
                {"AWB" , SaveType::NONE}, // NONE}            Worms Blast
                {"AWY" , SaveType::NONE}, // NONE}            Worms World Party
                {"ATE" , SaveType::EEPROM_4k}, // EEPROM_V122     WTA Tour Tennis
                {"ACI" , SaveType::EEPROM_4k}, // EEPROM_V122     WTA Tour Tennis Pocket
                {"AW8" , SaveType::EEPROM_4k}, // EEPROM_V122     WWE - Road to WrestleMania X8
                {"BWW" , SaveType::EEPROM_4k}, // EEPROM_V124     WWE Survivor Series
                {"AWF" , SaveType::NONE}, // NONE}            WWF - Road to WrestleMania
                {"AWV" , SaveType::EEPROM_4k}, // EEPROM_V122     X2 - Wolverine's Revenge
                {"AXI" , SaveType::NONE}, // NONE}            X-bladez - Inline Skater
                {"AXM" , SaveType::EEPROM_4k}, // EEPROM_V121     X-Men - Reign of Apocalypse
                {"B3X" , SaveType::EEPROM_4k}, // EEPROM_V124     X-Men - The Official Game
                {"BXM" , SaveType::NONE}, // NONE}            XS Moto
                {"AX3" , SaveType::EEPROM_4k}, // EEPROM_V121     xXx
                {"B64" , SaveType::NONE}, // NONE}            Yars' Revenge - Pong - Asteroids
                {"BYU" , SaveType::EEPROM_8k}, // EEPROM_V126     Yggdra Union - We'll Never Fight Alone
                {"BYV" , SaveType::SRAM_256K}, // SRAM_V103       Yo-Gi-Oh! Double Pack 2 - Destiny Board Traveler + Dungeon Dice Monsters
                {"KYG" , SaveType::EEPROM_4k}, // EEPROM_V124     Yoshi Topsy-Turvy
                {"A3A" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Yoshi's Island - Super Mario Advance 3
                {"AFU" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Youkaidou
                {"BYY" , SaveType::EEPROM_4k}, // EEPROM_V122     Yu Yu Hakusho - Spirit Detective
                {"BYD" , SaveType::SRAM_256K}, // SRAM_V103       Yu-Gi-Oh! - Destiny Board Traveler
                {"AY8" , SaveType::SRAM_256K}, // SRAM_V102       Yu-Gi-Oh! - Reshef of Destruction
                {"BY7" , SaveType::SRAM_256K}, // SRAM_V113       Yu-Gi-Oh! 7 Trials to Glory - World Championship Tournament 2005
                {"BYO" , SaveType::SRAM_256K}, // SRAM_V113       Yu-Gi-Oh! Day Of The Duelist - World Championship Tournament 2005
                {"BY2" , SaveType::SRAM_256K}, // SRAM_V102       Yu-Gi-Oh! Double Pack
                {"AY6" , SaveType::SRAM_256K}, // SRAM_V112       Yu-Gi-Oh! Duel Monsters 6 Expert 2
                {"BY3" , SaveType::SRAM_256K}, // SRAM_V113       Yu-Gi-Oh! Duel Monsters Expert 3
                {"BYI" , SaveType::SRAM_256K}, // SRAM_V113       Yu-Gi-Oh! Duel Monsters International 2
                {"AYD" , SaveType::Flash_512k_SST}, // FLASH_V126      Yu-Gi-Oh! Dungeon Dice Monsters
                {"BYG" , SaveType::SRAM_256K}, // SRAM_V113       Yu-Gi-Oh! GX - Duel Academy
                {"BYS" , SaveType::SRAM_256K}, // SRAM_V103       Yu-Gi-Oh! Sugoroku no Sugoroku
                {"AY5" , SaveType::SRAM_256K}, // SRAM_V112       Yu-Gi-Oh! The Eternal Duelist Soul
                {"AY7" , SaveType::SRAM_256K}, // SRAM_V102       Yu-Gi-Oh! The Sacred Cards
                {"BY6" , SaveType::SRAM_256K}, // SRAM_V103       Yu-Gi-Oh! Ultimate Masters 2006
                {"BYW" , SaveType::SRAM_256K}, // SRAM_V113       Yu-Gi-Oh! World Championship Tournament 2004
                {"AYW" , SaveType::SRAM_256K}, // SRAM_V113       Yu-Gi-Oh! Worldwide Edition - Stairway to the Destined Duel
                {"A4V" , SaveType::SRAM_256K}, // SRAM_V102       Yuujou no Victory Goal 4v4 Arashi - Get the Goal!!
                {"AUY" , SaveType::EEPROM_4k}, // EEPROM_V122     Yuureiyashiki no Nijuuyojikan
                {"BRG" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Yu-Yu-Hakusho - Tournament Tactics
                {"AZP" , SaveType::EEPROM_4k}, // EEPROM_V122     Zapper
                {"A4G" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     ZatchBell! - Electric Arena
                {"AGT" , SaveType::Flash_512k_Atmel}, // FLASH_V121      Zen-Nippon GT Senshuken
                {"A2Z" , SaveType::Flash_512k_SST}, // FLASH_V126      Zen-Nippon Shounen Soccer Taikai 2 - Mezase Nippon-ichi!
                {"AF3" , SaveType::EEPROM_4k_alt}, // EEPROM_V122     Zero One
                {"BZO" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Zero One SP
                {"AZT" , SaveType::Flash_512k_SST}, // FLASH_V124      Zero-Tours
                {"BJ3" , SaveType::SRAM_256K}, // SRAM_V113       Zettai Zetsumei - Dangerous Jiisan 3 Hateshinaki Mamonogatari
                {"BZD" , SaveType::SRAM_256K}, // SRAM_V113       Zettai Zetsumei Dangerous Jiisan - Shijou Saikyou no Togeza
                {"BZG" , SaveType::SRAM_256K}, // SRAM_V113       Zettai Zetsumei Dangerous Jiisan - Zettai Okujou Bai Orensu Kouchou
                {"BZ2" , SaveType::SRAM_256K}, // SRAM_V113       Zettai Zetsumei Den Chara Suji-Sa
                {"AZD" , SaveType::NONE}, // NONE}            Zidane Football Generation
                {"BZY" , SaveType::NONE}, // NONE}            Zoey 101
                {"AZ2" , SaveType::SRAM_256K}, // SRAM_V103       Zoids - Legacy
                {"ATZ" , SaveType::SRAM_256K}, // SRAM_V102       Zoids Saga
                {"BZF" , SaveType::SRAM_256K}, // SRAM_V103       Zoids Saga - Fuzors
                {"AZE" , SaveType::Flash_512k_SST}, // FLASH_V126      Zone of the Enders - The Fist of Mars
                {"ANC" , SaveType::EEPROM_4k}, // EEPROM_V122     ZooCube
                {"BMZ" , SaveType::EEPROM_4k_alt}, // EEPROM_V124     Zooo

            };
}

