#include "audio.h"
#include "folio.h"
#include "item.h"
#include "nodes.h"

#include "rw_semantic_data.h"
#include "sdk_audio_compat.h"

Err
open_audio_folio_compat(void)
{
  Item folio_item;

  folio_item = FindNamedItem(
    MKNODEID(KERNELNODE, FOLIONODE), "audio");
  folio_item = OpenItem(folio_item, 0);
  if(folio_item < 0)
    {
      return folio_item;
    }

  AudioFolioItem = folio_item;
  AudioBase = (AudioFolio *)LookupItem(folio_item);
  return 0;
}
