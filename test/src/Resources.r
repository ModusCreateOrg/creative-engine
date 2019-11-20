#
### FONTS
#
PATH resources/charset
BITMAP charset_8x8.bmp
BITMAP charset_16x16.bmp

#
### SPLASH, TITLE, ETC.
#
PATH resources/
BITMAP splash.bmp
BITMAP splash_sprites.bmp
BITMAP title.bmp
PALETTE test.pal

#
### ENVIRONMENT/OBJECTS
#
PATH resources/sprites/environment
SPRITESHEET 32x32 dungeon_tileset_objects.bmp

#
### HERO
#
PATH resources/sprites/
SPRITESHEET 64x64 chara_hero.bmp
SPRITESHEET 32x32 chara_hero_heal_effect.bmp
SPRITESHEET 32x32 chara_hero_spell_effect.bmp

#
### ENEMIES
#
PATH resources/sprites/enemies
SPRITESHEET 64x64 chara_bat.bmp
SPRITESHEET 64x64 chara_goblin.bmp
SPRITESHEET 64x64 chara_goblin_sniper.bmp
SPRITESHEET 64x64 chara_orc.bmp
SPRITESHEET 64x64 chara_rat.bmp
SPRITESHEET 64x64 chara_slime.bmp
SPRITESHEET 64x64 chara_spider.bmp
SPRITESHEET 64x64 chara_troll.bmp
SPRITESHEET 64x64 projectile_arrow.bmp
SPRITESHEET 64x64 enemy_death.bmp

#
### MID BOSSES
#
PATH resources/sprites/mid_boss
SPRITESHEET 64x64 mid_boss_death_explosion.bmp
SPRITESHEET 128x128 mid_boss_earth_brown.bmp
SPRITESHEET 32x32 mid_boss_earth_projectile.bmp
SPRITESHEET 128x128 mid_boss_energy.bmp
SPRITESHEET 32x32 mid_boss_energy_projectile.bmp
SPRITESHEET 128x128 mid_boss_fire.bmp
SPRITESHEET 32x32 mid_boss_fire_projectile.bmp
SPRITESHEET 128x128 mid_boss_water.bmp
SPRITESHEET 32x32 mid_boss_water_projectile.bmp

#
### TILEMAPS
#
# PATH resources/tilemaps/DevDungeon_0
# TILEMAP DevDungeon_0.filelist.txt
PATH resources/tilemaps-new/DevDungeon_0
TMX DevDungeo_0.txt
PATH resources/tilemaps-new/Dungeon_1
TMX Dungeon_1.txt

########### SOUND :: MUSIC ###########
PATH resources/music/
RAW EmptySong.xm
RAW Countryside.xm
RAW Cyberpunk.xm
RAW GameOver.xm
RAW Glacial_mountains.xm
RAW Main_Menu.xm
RAW Spaaace.xm
RAW UnderWaterFantasy.xm
RAW Under_Water.xm
RAW EnterCredits.xm

########### SOUND :: SFX ###########
PATH resources/sound_effects/
RAW SFX_rotate_block_right.wav
RAW SFX_rotate_block_left.wav
RAW SFX_good_drop_block.wav
RAW SFX_bad_drop_block.wav
RAW SFX_move_block.wav
RAW SFX_score_combo.wav
RAW SFX_option_select.wav
RAW SFX_explode_block.wav
RAW SFX_next_level.wav
RAW SFX_next_stage.wav
RAW SFX_save_game.wav
