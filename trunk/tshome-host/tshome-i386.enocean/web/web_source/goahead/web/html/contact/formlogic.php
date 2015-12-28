<?

$output = '';

$required = array('email','name','message');

$errors = array();

if (isset($_REQUEST['submit'])) {

	foreach ($required as $field) {

		if (empty($_REQUEST[$field])) {

			$errors[$field] = 'this is required';

		}
	}

	if (count($errors)<1) {

//		Send an email

		$output.= '<p class="feedback">';
		$output.= 'Thank you for getting in touch';
		$output.= '</p>';

		echo $output;
		return;

	}
}

$output.= '
<form method="post" id="contactform" action="'.$_SERVER['PHP_SELF'].'">';

$fields = array(
	'name'		=>	'text',
	'email'		=>	'text',
	'message'	=>	'textarea'
);

foreach ($fields as $field => $type) {

	$output.= '
<p>';
	$output.= '
<label for="'.$field.'">';
	$output.= ucwords($field);
	if (isset($errors[$field])) {
		$output.= ' <strong class="error">is required</strong>';
	}
	$output.= '</label>';

	switch ($type) {

		case 'textarea':
			$output.= '
<textarea name="'.$field.'" id="'.$field.'" cols="30" rows="10">';
			if (isset($_REQUEST[$field])) {
				$output.= htmlspecialchars($_REQUEST[$field]);
			}
			$output.= '
</textarea>';
		break;

		default:
			$output.= '
<input type="text" name="'.$field.'" id="'.$field.'"';
			if (isset($_REQUEST[$field])) {
				$output.= ' value="'.htmlspecialchars($_REQUEST[$field]).'"';
			}
			$output.= ' />';
		break;

	}

	$output.= '
</p>';

}

$output.= '
<p>
<input type="submit" name="submit" value="Submit" />
</p>
</form>
';

echo $output;

?>