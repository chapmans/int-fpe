$(document).ready(function(){
  
  var allow = false;

  $('.go').attr('disabled', 'disabled');

  /* Validating inputs */
  function letSubmit(ok) {
    if (!ok) {
      $('.go').attr('disabled', 'disabled');
      allow = false;
    } else {
      $('.go').removeAttr('disabled');
      allow = true;
    }
  }

  $('input').keyup(function() {
    var keyArr = $('#inputKey').val().split('');
    var okKey =  _.every(keyArr, passHex) && (keyArr.length === 32);
    var okRegex = $('#inputRegex').val().length > 0;
    var okMessage = $('#inputMessage').val().length > 0;
    letSubmit(okKey && okRegex && okMessage);
  });


  function passHex(ch) {
    return /[0-9A-Fa-f]/.test(ch);
  }



  /* AJAX request to get encrypted string */

  $('.go').click(makeRequest);
  $('input').keyup(function(e) {
    if (e.keyCode == 13 && allow) makeRequest();
  });

  function makeRequest() {
    $.ajax({
      type: 'POST',
      url: 'fpe-encrypt',
      data: {
        regex: $('#inputRegex').val(),
        msg: $('#inputMessage').val(),
        key: $('#inputKey').val()
      },
      success: function(ret) {
        $('.output').html("Your encrypted string: " + ret);
      }
    });
  }

});