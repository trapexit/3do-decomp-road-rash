#ifndef ROADRASH_RACE_OBJECT_LIFECYCLE_H
#define ROADRASH_RACE_OBJECT_LIFECYCLE_H

struct RacerEntity;

void
schedule_race_object_removal(void *object);
void
dispose_pending_race_objects(void);
void
update_scheduled_race_object(struct RacerEntity *object,
                             int                 tick,
                             int                 enforce_visibility_window);

#endif
