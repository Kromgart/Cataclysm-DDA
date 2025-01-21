#include "fungal_effects.h"

#include <algorithm>
#include <memory>
#include <ostream>

#include "calendar.h"
#include "character.h"
#include "coordinates.h"
#include "creature.h"
#include "creature_tracker.h"
#include "debug.h"
#include "enums.h"
#include "field_type.h"
#include "game.h"
#include "item.h"
#include "item_stack.h"
#include "map.h"
#include "map_iterator.h"
#include "mapdata.h"
#include "messages.h"
#include "monster.h"
#include "mtype.h"
#include "point.h"
#include "rng.h"
#include "string_formatter.h"
#include "translations.h"
#include "type_id.h"

static const efftype_id effect_spores( "spores" );
static const efftype_id effect_stunned( "stunned" );

static const furn_str_id furn_f_flower_fungal( "f_flower_fungal" );
static const furn_str_id furn_f_fungal_clump( "f_fungal_clump" );
static const furn_str_id furn_f_fungal_mass( "f_fungal_mass" );

static const itype_id itype_fungal_seeds( "fungal_seeds" );

static const mtype_id mon_fungal_blossom( "mon_fungal_blossom" );
static const mtype_id mon_spore( "mon_spore" );

static const skill_id skill_melee( "melee" );

static const species_id species_FUNGUS( "FUNGUS" );

static const ter_str_id ter_t_fungus( "t_fungus" );
static const ter_str_id ter_t_fungus_floor_in( "t_fungus_floor_in" );
static const ter_str_id ter_t_fungus_floor_out( "t_fungus_floor_out" );
static const ter_str_id ter_t_fungus_floor_sup( "t_fungus_floor_sup" );
static const ter_str_id ter_t_fungus_mound( "t_fungus_mound" );
static const ter_str_id ter_t_fungus_wall( "t_fungus_wall" );
static const ter_str_id ter_t_marloss( "t_marloss" );
static const ter_str_id ter_t_marloss_tree( "t_marloss_tree" );
static const ter_str_id ter_t_shrub_fungal( "t_shrub_fungal" );
static const ter_str_id ter_t_tree_fungal( "t_tree_fungal" );
static const ter_str_id ter_t_tree_fungal_young( "t_tree_fungal_young" );

static const trait_id trait_TAIL_CATTLE( "TAIL_CATTLE" );
static const trait_id trait_THRESH_MYCUS( "THRESH_MYCUS" );

void fungal_effects::fungalize( const tripoint_bub_ms &p, Creature *origin, double spore_chance )
{
    Character &player_character = get_player_character();
    if( monster *const mon_ptr = get_creature_tracker().creature_at<monster>( p ) ) {
        monster &critter = *mon_ptr;
        if( !critter.type->in_species( species_FUNGUS ) ) {
            add_msg_if_player_sees( p, _( "The %s is covered in tiny spores!" ), critter.name() );
        }
        if( !critter.make_fungus() ) {
            // Don't insta-kill non-fungables. Jabberwocks, for example
            critter.add_effect( effect_stunned, rng( 1_turns, 3_turns ) );
            critter.apply_damage( origin, bodypart_id( "torso" ), rng( 25, 50 ) );
        }
    } else if( player_character.pos_bub() == p ) {
        // TODO: Make this accept NPCs when they understand fungals
        ///\EFFECT_DEX increases chance of knocking fungal spores away with your TAIL_CATTLE
        ///\EFFECT_MELEE increases chance of knocking fungal sports away with your TAIL_CATTLE
        if( player_character.has_trait( trait_TAIL_CATTLE ) &&
            one_in( 20 - player_character.dex_cur - player_character.get_skill_level( skill_melee ) ) ) {
            add_msg( _( "The spores land on you, but you quickly swat them off with your tail!" ) );
            return;
        }
        // Spores hit the player--is there any hope?
        bool hit = false;
        hit |= one_in( 4 ) &&
               player_character.add_env_effect( effect_spores, bodypart_id( "head" ), 3, 9_minutes,
                                                bodypart_id( "head" ) );
        hit |= one_in( 2 ) &&
               player_character.add_env_effect( effect_spores, bodypart_id( "torso" ), 3, 9_minutes,
                                                bodypart_id( "torso" ) );
        hit |= one_in( 4 ) &&
               player_character.add_env_effect( effect_spores, bodypart_id( "arm_l" ), 3, 9_minutes,
                                                bodypart_id( "arm_l" ) );
        hit |= one_in( 4 ) &&
               player_character.add_env_effect( effect_spores, bodypart_id( "arm_r" ), 3, 9_minutes,
                                                bodypart_id( "arm_r" ) );
        hit |= one_in( 4 ) &&
               player_character.add_env_effect( effect_spores, bodypart_id( "leg_l" ), 3, 9_minutes,
                                                bodypart_id( "leg_l" ) );
        hit |= one_in( 4 ) &&
               player_character.add_env_effect( effect_spores, bodypart_id( "leg_r" ), 3, 9_minutes,
                                                bodypart_id( "leg_r" ) );
        if( hit ) {
            add_msg( m_warning, _( "You're covered in tiny spores!" ) );
        }
    } else if( g->num_creatures() < 250 && x_in_y( spore_chance, 1.0 ) ) { // Spawn a spore
        if( monster *const spore = g->place_critter_at( mon_spore, p ) ) {
            monster *origin_mon = dynamic_cast<monster *>( origin );
            if( origin_mon != nullptr ) {
                spore->make_ally( *origin_mon );
            } else if( origin != nullptr && origin->is_avatar() &&
                       player_character.has_trait( trait_THRESH_MYCUS ) ) {
                spore->friendly = 1000;
            }
        }
    } else {
        spread_fungus( p );
    }
}

void fungal_effects::create_spores( const tripoint_bub_ms &p, Creature *origin )
{
    for( const tripoint_bub_ms &tmp : get_map().points_in_radius( p, 1 ) ) {
        fungalize( tmp, origin, 0.25 );
    }
}

void fungal_effects::marlossify( const tripoint_bub_ms & )
{
}

void fungal_effects::spread_fungus_one_tile( const tripoint_bub_ms &, const int )
{
}

void fungal_effects::spread_fungus( const tripoint_bub_ms & )
{
}
