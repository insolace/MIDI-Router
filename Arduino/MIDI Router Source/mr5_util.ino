
// for any given X value of the touch, return the column of our grid
int getTouchCol(long x){
  if (x > (WIDE-3) - ((WIDE - cOffset) / columns)) {
    return(6);
  } else if (x > (WIDE-3) - (((WIDE - cOffset) / columns))*2) {
    return(5);
  } else if (x > (WIDE-3) - (((WIDE - cOffset) / columns))*3) {
    return(4);
  } else if (x > (WIDE-3) - (((WIDE - cOffset) / columns))*4) {
    return(3);
  } else if (x > (WIDE-3) - (((WIDE - cOffset) / columns))*5) {
    return(2);
  } else {
    return(1);
  }
}

// for any given Y value of the touch, return the row of our grid
int getTouchRow(long y){
  if (y > (TALL+3) - ((TALL - rOffset) / rows)) {
    return(6);
  } else if (y > (TALL+3) - (((TALL - rOffset) / rows))*2) {
    return(5);
  } else if (y > (TALL+3) - (((TALL - rOffset) / rows))*3) {
    return(4);
  } else if (y > (TALL+3) - (((TALL - rOffset) / rows))*4) {
    return(3);
  } else if (y > (TALL+3) - (((TALL - rOffset) / rows))*5) {
    return(2);
  } else {
    return(1);
  }
}



