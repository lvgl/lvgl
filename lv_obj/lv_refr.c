/**
 * @file lv_refr.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
#include "lv_conf.h"
#include "misc/os/ptask.h"
#include "misc/mem/fifo.h"
#include "lv_refr.h"
#include "lv_vdb.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct
{
    area_t area;
    uint8_t joined;
}lv_join_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_refr_task(void);
static void lv_refr_join_area(lv_join_t * area_a, uint32_t inv_num);
static void lv_refr_areas(lv_join_t * area_a, uint32_t area_num);
#if LV_VDB_SIZE == 0
static void lv_refr_area_no_vdb(const area_t * area_p);
#else
static void lv_refr_area_with_vdb(const area_t * area_p);
static void lv_refr_area_part_vdb(const area_t * area_p);
#endif
static lv_obj_t* lv_refr_get_top_obj(const area_t * area_p, lv_obj_t* obj_dp);
static void lv_refr_make(lv_obj_t* top_p, const area_t * mask_p);
static void lv_refr_obj(lv_obj_t* obj_dp, const area_t * mask_ori_p);

/**********************
 *  STATIC VARIABLES
 **********************/
fifo_t fifo_inv;
area_t fifo_inv_buf[LV_INV_FIFO_SIZE];

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the screen refresh subsystem
 */
void lv_refr_init(void)
{    
    fifo_init(&fifo_inv, fifo_inv_buf, sizeof(area_t), LV_INV_FIFO_SIZE);
    
    ptask_t* task;
    task = ptask_create(lv_refr_task, LV_REFR_PERIOD, PTASK_PRIO_MID);
    dm_assert(task);
    
}

/**
 * Invalidate an area
 * @param area_p pointer to area which should be invalidated
 */
void lv_inv_area(const area_t * area_p)
{
    area_t scr_area;
    scr_area.x1 = 0;
    scr_area.y1 = 0;
    scr_area.x2 = LV_HOR_RES - 1;
    scr_area.y2 = LV_VER_RES - 1;
    
    area_t com_area;    
    bool suc;

    suc = area_union(&com_area, area_p, &scr_area);

    /*The area is truncated to the screen*/
    if(suc != false)
    {
#if LV_DOWNSCALE == 2
    	/*Rounding*/
    	com_area.x1 = com_area.x1 & (~0x1);
    	com_area.y1 = com_area.y1 & (~0x1);
    	com_area.x2 = com_area.x2 | 0x1;
    	com_area.y2 = com_area.y2 | 0x1;
#elif LV_DOWNSCALE == 4
    	com_area.x1 = com_area.x1 & (~0x3);
    	com_area.y1 = com_area.y1 & (~0x3);
    	com_area.x2 = com_area.x2 | 0x3;
    	com_area.y2 = com_area.y2 | 0x3;
#endif
        /*Save the area*/
        suc = fifo_push(&fifo_inv, &com_area); 

        /* There is no place for the new area 
         * clear the fifo and add the whole screen*/
        if(suc == false)
        {
            fifo_clear(&fifo_inv);
            fifo_push(&fifo_inv, &scr_area);        
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Called periodically to handle the refreshing
 */
static void lv_refr_task(void)
{
    lv_join_t area_tmp[LV_INV_FIFO_SIZE];
    
    memset(area_tmp, 0, sizeof(area_tmp));
    
    /*Read all data from the fifo_inv*/
    uint32_t inv_num;
    bool suc;
    for(inv_num = 0; inv_num < LV_INV_FIFO_SIZE; inv_num++)
    {
         suc = fifo_pop(&fifo_inv, &area_tmp[inv_num].area); 
         
         if(suc == false) /*Break id the fifo is empty*/
         {
             break;
         }
    }
 
    lv_refr_join_area(area_tmp, inv_num);
    
    lv_refr_areas(area_tmp, inv_num);
}


/**
 * Join the areas which has got common parts
 * @param join_a an array of areas to join
 * @param inv_num item number of the array
 */
static void lv_refr_join_area(lv_join_t * area_a, uint32_t area_num)
{
    uint32_t join_from;
    uint32_t join_in;
    area_t joined_area;
    for(join_in = 0; join_in < area_num; join_in++) {
        if(area_a[join_in].joined != 0) continue;
        
        /*Check all areas to join them in 'join_in'*/
        for(join_from = 0; join_from < area_num; join_from++) {
            /*Handle only unjoined areas and ignore itself*/
            if(area_a[join_from].joined != 0 || join_in == join_from) {
                continue;
            }

            /*Check if the areas are on each other*/
            if(area_is_on(&area_a[join_in].area, 
                          &area_a[join_from].area) == false)
            {
                continue;
            }
            
            area_join(&joined_area, &area_a[join_in].area,
                                    &area_a[join_from].area);

            /*Join two area only if the joined area size is smaller*/
            if(area_get_size(&joined_area) < 
             (area_get_size(&area_a[join_in].area) + area_get_size(&area_a[join_from].area))) {
                area_cpy(&area_a[join_in].area, &joined_area);

                /*Mark 'join_form' is joined into 'join_in'*/
                area_a[join_from].joined = 1;     
            }
        }   
    }
}

/**
 * Refresh the joined areas
 * @param area_a array of joined invalid areas
 * @param area_num item number of the array
 */
static void lv_refr_areas(lv_join_t * area_a, uint32_t area_num)
{
    uint32_t i;
    
    for(i = 0; i < area_num; i++) {
        /*Refresh the unjoined areas*/
        if(area_a[i].joined == 0) {   
            /*If there is no VDB do simple drawing*/
#if LV_VDB_SIZE == 0
            lv_refr_area_no_vdb(&area_a[i].area);
#else
            /*If VDB is used...*/
            lv_refr_area_with_vdb(&area_a[i].area);
#endif
            
        }
    }
}

#if LV_VDB_SIZE == 0
/**
 * Refresh an area if there is no Virtual Display Buffer
 * @param area_p pointer to an area to refresh
 */
static void lv_refr_area_no_vdb(const area_t * area_p)
{
    lv_obj_t* top_p;
    
    /*Get top object which is not covered by others*/    
    top_p = lv_refr_get_top_obj(area_p, lv_scr_act());
    
    /*Do the refreshing*/
    lv_refr_make(top_p, area_p);
}

#else

/**
 * Refresh an area if there is Virtual Display Buffer
 * @param area_p  pointer to an area to refresh
 */
static void lv_refr_area_with_vdb(const area_t * area_p)
{
    lv_vdb_t * vdb_p = lv_vdb_get();
    
    /*Always use the full row*/
    vdb_p->vdb_area.x1 = area_p->x1;
    vdb_p->vdb_area.y1 = area_p->y1;
    vdb_p->vdb_area.x2 = area_p->x2;

    /*Calculate the max row num*/
    uint32_t max_row = (uint32_t) LV_VDB_SIZE / (vdb_p->vdb_area.x2 - vdb_p->vdb_area.x1 + 1);
    if(max_row > area_get_height(area_p)) max_row = area_get_height(area_p);
    
    /*Round the row number with downscale*/
#if LV_DOWNSCALE == 2
    max_row &= (~0x1);
#elif LV_DOWNSCALE == 4
    max_row &= (~0x3);
#endif

    /*Refresh all rows*/
    cord_t row = area_p->y1;

    for(row = area_p->y1; row  + max_row - 1 <= area_p->y2; row += max_row)  {
        /*Calc. the next y coordinates of VDB*/
        vdb_p->vdb_area.y1 = row;
        vdb_p->vdb_area.y2 = row + max_row - 1;

        lv_refr_area_part_vdb(area_p);
    }
    
    /*If the last y coordinates are not handled yet ...*/
    if(area_p->y2 != vdb_p->vdb_area.y2) {
        /*Calc. the next y coordinates of VDB*/
        vdb_p->vdb_area.y1 = row;
        vdb_p->vdb_area.y2 = area_p->y2;

        /*Refresh this part too*/
        lv_refr_area_part_vdb(area_p);
    }
}

/**
 * Refresh a part of an area which is on the actual Virtual Display Buffer
 * @param area_p pointer to an area to refresh
 */
static void lv_refr_area_part_vdb(const area_t * area_p)
{
    lv_vdb_t * vdb_p = lv_vdb_get();
    lv_obj_t* top_p;

    /*Get the new mask from the original area and the act. VDB
     It will be a part of 'area_p'*/
    area_t start_mask;
    area_union(&start_mask, area_p, &vdb_p->vdb_area);

    /*Get the most top object which is not covered by others*/
    top_p = lv_refr_get_top_obj(&start_mask, lv_scr_act());

    /*Do the refreshing*/
    lv_refr_make(top_p, &start_mask);
            
    /*Flush the content of the VDB*/ 
    lv_vdb_flush();
}

#endif /*LV_VDB_SIZE == 0*/

/**
 * Search the most top object which fully covers an area
 * @param area_p pointer to an area
 * @param obj_dp the first object to start the searching (typically a screen)
 * @return 
 */
static lv_obj_t* lv_refr_get_top_obj(const area_t * area_p, lv_obj_t* obj_dp)
{
    lv_obj_t* i;
    lv_obj_t* found_p = NULL;
    
    /*If this object is fully cover the draw area check the children too */
    if(obj_dp->opa == OPA_COVER &&
       obj_dp->hidden == 0 &&
	   LV_SA(obj_dp, lv_objs_t)->transp == 0 &&
       obj_dp->design_f(obj_dp, area_p, LV_DESIGN_COVER_CHK) != false)
    {
        LL_READ(obj_dp->child_ll, i)        {
            found_p = lv_refr_get_top_obj(area_p, i);
            
            /*If a children is ok then break*/
            if(found_p != NULL) {
                break;
            }
        }
        
        /*If there is no better children use this object*/
        if(found_p == NULL) {
            found_p = obj_dp;
        }
    }
    
    return found_p;
}

/**
 * Make the refreshing from an object. Draw all its children and the yungers too. 
 * @param top_p pointer to an objects. Start the drawing from it.
 * @param mask_p pointer to an area, the objects will be drawn only here
 */
static void lv_refr_make(lv_obj_t* top_p, const area_t * mask_p)
{
    /* Normally always will be a top_obj (at least the screen)
     * but in special cases (e.g. if the screen has alpha) it won't.
     * In this case use the screen directly */
    if(top_p == NULL) top_p = lv_scr_act();
    
    /*Refresh the top object and its children*/
    lv_refr_obj(top_p, mask_p);
    
    /*Draw the 'younger' objects because they can be on top_obj */
    lv_obj_t* par_dp;
    lv_obj_t* i;
    lv_obj_t* border_p = top_p;

    par_dp = lv_obj_get_parent(top_p);

    /*Do until not reach the screen*/
    while(par_dp != NULL) {
        /*object before border_p has to be redrawn*/
        i = ll_get_prev(&(par_dp->child_ll), border_p);

        while(i != NULL) { 
            /*Refresh the objects*/
            lv_refr_obj(i, mask_p);
            i = ll_get_prev(&(par_dp->child_ll), i);
        }  
        
        /*The new border will be there last parents,
         *so the 'younger' brothers of parent will be refreshed*/
        border_p = par_dp;
        /*Go a level deeper*/
        par_dp = lv_obj_get_parent(par_dp);
    }
}

/**
 * Refresh an object an all of its children. (Called recursively)
 * @param obj_dp pointer to an object to refresh
 * @param mask_ori_p pointer to an area, the objects will be drawn only here
 */
static void lv_refr_obj(lv_obj_t* obj_dp, const area_t * mask_ori_p)
{
    /*Do not refresh hidden objects*/
    if(obj_dp->hidden != 0) return;
    
    bool union_ok;  /* Store the return value of area_union */
    /* Truncate the original mask to the coordinates of the parent
     * because the parent and its children are visible only here */
    area_t mask_parent;
    union_ok = area_union(&mask_parent, mask_ori_p, &obj_dp->cords);
    
    /*Draw the parent and its children only if they ore on 'mask_parent'*/
    if(union_ok != false) {

        /* Redraw the object */    
        if(obj_dp->opa != OPA_TRANSP && LV_SA(obj_dp, lv_objs_t)->transp == 0) {
            obj_dp->design_f(obj_dp, &mask_parent, LV_DESIGN_DRAW_MAIN);
        }

        area_t mask_child; /*Mask from obj_dp and its child*/
        lv_obj_t* child_p;    
        LL_READ_BACK(obj_dp->child_ll, child_p)
        {   
            /* Get the union (common parts) of original mask (from obj_dp)
             * and its child */
            union_ok = area_union(&mask_child, &mask_parent, &child_p->cords);

            /*If the parent and the child has common area then refresh the child */        
            if(union_ok) {
                /*Refresh the next children*/
                lv_refr_obj(child_p, &mask_child);
            }
        }

        /* If all the children are redrawn call make 'post draw' design */
		if(obj_dp->opa != OPA_TRANSP && LV_SA(obj_dp, lv_objs_t)->transp == 0) {
		  obj_dp->design_f(obj_dp, &mask_parent, LV_DESIGN_DRAW_POST);
		}
    }
}
