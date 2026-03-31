#include "level.h"

#include "level_includes.h"

LevelDefinition levels[] = {
    {
        .level_name = "Stereo Madness",
        .data_ptr  = StereoMadness_gmd,
        .data_size = StereoMadness_gmd_size
    }, 
    {
        .level_name = "Back On Track",
        .data_ptr  = BackOnTrack_gmd,
        .data_size = BackOnTrack_gmd_size
    }, 
    {
        .level_name = "Polargeist",
        .data_ptr  = Polargeist_gmd,
        .data_size = Polargeist_gmd_size
    }, 
    {
        .level_name = "Dry Out",
        .data_ptr  = DryOut_gmd,
        .data_size = DryOut_gmd_size
    }, 
    {
        .level_name = "Base After Base",
        .data_ptr  = BaseAfterBase_gmd,
        .data_size = BaseAfterBase_gmd_size
    }, 
    {
        .level_name = "Cant Let Go",
        .data_ptr  = CantLetGo_gmd,
        .data_size = CantLetGo_gmd_size
    }, 
    {
        .level_name = "Jumper",
        .data_ptr  = Jumper_gmd,
        .data_size = Jumper_gmd_size
    }, 
    {
        .level_name = "Time Machine",
        .data_ptr  = TimeMachine_gmd,
        .data_size = TimeMachine_gmd_size
    }, 
    {
        .level_name = "Cycles",
        .data_ptr  = Cycles_gmd,
        .data_size = Cycles_gmd_size
    }, 
    {
        .level_name = "xStep",
        .data_ptr = xStep_gmd,
        .data_size = xStep_gmd_size
    },
    {
        .level_name = "Clutterfunk",
        .data_ptr = Clutterfunk_gmd,
        .data_size = Clutterfunk_gmd_size
    },
    {
        .level_name = "Theory of Everything",
        .data_ptr = TheoryOfEverything_gmd,
        .data_size = TheoryOfEverything_gmd_size
    },
    {
        .level_name = "Electroman Adventures",
        .data_ptr = Electroman_gmd,
        .data_size = Electroman_gmd_size
    },
    {
        .level_name = "Clubstep",
        .data_ptr = Clubstep_gmd,
        .data_size = Clubstep_gmd_size
    },
    {
        .level_name = "Electrodynamix",
        .data_ptr = Electrodynamix_gmd,
        .data_size = Electrodynamix_gmd_size
    },
    {
        .level_name = "Hexagon Force",
        .data_ptr = HexagonForce_gmd,
        .data_size = HexagonForce_gmd_size
    },
    {
        .level_name = "Blast Processing",
        .data_ptr = BlastProcessing_gmd,
        .data_size = BlastProcessing_gmd_size
    },
    {
        .level_name = "Theory of Everything 2",
        .data_ptr = TheoryofEverything2_gmd,
        .data_size = TheoryofEverything2_gmd_size
    },
    {
        .level_name = "Geometrical Dominator",
        .data_ptr = GeometricalDominator_gmd,
        .data_size = GeometricalDominator_gmd_size
    },
    {
        .level_name = "Deadlocked",
        .data_ptr = Deadlocked_gmd,
        .data_size = Deadlocked_gmd_size
    },
    {
        .level_name = "Level Easy",
        .data_ptr  = LevelEasy_gmd,
        .data_size = LevelEasy_gmd_size
    },
    {
        .level_name = "THE LIGHTNING ROAD",
        .data_ptr  = TheLightningRoad_gmd,
        .data_size = TheLightningRoad_gmd_size
    },
    {
        .level_name = "The Nightmare",
        .data_ptr  = TheNightmare_gmd,
        .data_size = TheNightmare_gmd_size
    },
    {
        .level_name = "DEMON PARK",
        .data_ptr  = DemonPark_gmd,
        .data_size = DemonPark_gmd_size
    },
    {
        .level_name = "auto jumper",
        .data_ptr  = AutoJumper_gmd,
        .data_size = AutoJumper_gmd_size
    }
};

SongDefinition songs[] = {
    {
        .song_name = "StereoMadness.mp3",
        .tempo = 160
    },
    {
        .song_name = "BackOnTrack.mp3",
        .tempo = 142
    },
    {
        .song_name = "Polargeist.mp3",
        .tempo = 175
    },
    {
        .song_name = "DryOut.mp3",
        .tempo = 145
    },
    {
        .song_name = "BaseAfterBase.mp3",
        .tempo = 141
    },
    {
        .song_name = "CantLetGo.mp3",
        .tempo = 170
    },
    {
        .song_name = "Jumper.mp3",
        .tempo = 175
    },
    {
        .song_name = "TimeMachine.mp3",
        .tempo = 143
    },
    {
        .song_name = "Cycles.mp3",
        .tempo = 140
    },
    {
        .song_name = "xStep.mp3",
        .tempo = 130
    },
    {
        .song_name = "Clutterfunk.mp3",
        .tempo = 140
    },
    {
        .song_name = "TheoryOfEverything.mp3",
        .tempo = 132
    },
    {
        .song_name = "Electroman.mp3",
        .tempo = 170
    },
    {
        .song_name = "Clubstep.mp3",
        .tempo = 128
    },
    {
        .song_name = "Electrodynamix.mp3",
        .tempo = 127
    },
    {
        .song_name = "HexagonForce.mp3",
        .tempo = 162
    },
    {
        .song_name = "BlastProcessing.mp3",
        .tempo = 135
    },
    {
        .song_name = "TheoryOfEverything2.mp3",
        .tempo = 132
    },
    {
        .song_name = "GeometricalDominator.mp3",
        .tempo = 148
    },
    {
        .song_name = "Deadlocked.mp3",
        .tempo = 135
    }
};

