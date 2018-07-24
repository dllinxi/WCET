int expensive ()
{
  volatile int x, w;
  int y, z;
  z = x + y * y + w * x;
  return z;
}

int cheap ()
{
  volatile int x;
  int  z;
  z = x;
  return z;
}


int main (void) 
{
  volatile int p, q, r, s, t;
  int i, j, k;
  int init;

  if (init) 
    {
      expensive(); 
    }
  else
    {
      cheap();
    }

  if (t < 10)
    {
      cheap();
    }
    else
    {
      expensive();
    }

  if (!r)
    {
      for (i = 0; i < 300; i++) 
        expensive();
    }

  for (i = 10; i < 100; i++)
    {
      if (t < i)        
        {
          expensive();
        }
      if (!init) 
        {
          expensive();
        }
      else
        {
          cheap();
        }
    }

  if (p == 13)
    {
      for (j = 0; j < 10; j++)
        {
          if (q > 26)       
            {
              expensive();
            }
          else
            {
              cheap();
            }
        }
    }
  else
    {
      if (r)          
        {
          for (i = 0; i < 10; i++)
            {
              for (k = i; k < 10; k++)
                {
                  if (s && t)
                    {
                      for (j = 0; j < 4; j++)
                        expensive();
                    }
                  else
                    {
                      expensive();
                    }
                }
            }
        }
      else 
        {
          cheap();
        }
    }

  return 0;
}
