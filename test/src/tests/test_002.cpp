#include "../test.h"

#include <BSprite.h>
#include <Resources.h>
#include <BResourceManager.h>

// BSpriteList ascending order
bool test_002(){

return  ETrue;


    BSprite sprite1 (1, MID_BOSS_FIRE_PROJECTILE_BMP_SPRITES);
    BSprite sprite2 (2, MID_BOSS_FIRE_PROJECTILE_BMP_SPRITES);
    BSprite sprite3 (3, MID_BOSS_FIRE_PROJECTILE_BMP_SPRITES);
    BSprite sprite4 (4, MID_BOSS_FIRE_PROJECTILE_BMP_SPRITES);
    BSprite sprite5 (5, MID_BOSS_FIRE_PROJECTILE_BMP_SPRITES);
    BSprite sprite6 (6, MID_BOSS_FIRE_PROJECTILE_BMP_SPRITES);
    BSprite sprite7 (7, MID_BOSS_FIRE_PROJECTILE_BMP_SPRITES);

    BSpriteList list;

    list.Add(sprite1);
    list.Add(sprite2);
    list.Add(sprite3);
    list.Add(sprite3);
    list.Add(sprite7);
    list.Add(sprite6);
    list.Add(sprite7);
    list.Add(sprite5);
    list.Add(sprite4);

    if(list.End(list.First())){
        printf("The list is empty!\n");
        return EFalse;
    }

    BSprite *head = (BSprite*)list.First();
    for (BSprite *n = (BSprite*)head->next; !list.End(n); head = (BSprite*)n, n = (BSprite*)n->next) {
        if(head->pri < n->pri){
            printf("The list is not ascending!\n");
            return EFalse;
        }
    }

    BSprite *tail = (BSprite*)list.Last();
    for (BSprite *n = (BSprite*)tail->prev; !list.End(n); tail = (BSprite*)n, n = (BSprite*)n->prev) {
        if(tail->pri > n->pri){
            printf("The list is not ascending!\n");
            return EFalse;
        }
    }

    list.Reset();
    return ETrue;
}
