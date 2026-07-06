int
absolute_int_difference(int first,
                        int second)
{
  int diff;

  diff = second - first;
  if(diff < 0)
    {
      diff = -diff;
    }
  return diff;
}
