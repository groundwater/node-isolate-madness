
function prime(n) {
  for (var i = 2; i<n-1; i++)
    if (n % i === 0) return i;
  return false;
}

exports = function(i){
  i = parseInt(i, 10);

  while(prime(--i));

  return String(i);
}
