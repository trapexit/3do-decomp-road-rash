/* Reconstructed from the original function at 0x0003339C. */

int
initialize_music_context_from_tags(int tag_list);

int
initialize_default_music_context(void)
{
  return initialize_music_context_from_tags(0);
}
