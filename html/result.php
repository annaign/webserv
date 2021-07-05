#! /usr/bin/php

<html>
 <head>
  <title>PHP test</title>
 </head>
<body>
<br>
<h2>Dynamically generated page.</h2>

<?php
parse_str(getenv("QUERY_STRING"), $_GET);
echo "<h2><p>Now we know your email is: </p><p>" . $_GET['email'] . "</p></h2>";
?>

<hr>
</body>
</html>