jQuery(function($) {
  function loadTablutGame(container, options, callback) {
    var tablut = $('.board', container).tablut(options);

    $('.back', container).click(function() {
      tablut.transitionBackward();
      $('.annot', container).text( tablut.annotation() );
      return false;
    });

    $('.next', container).click(function() {
      tablut.transitionForward();
      $('.annot', container).text( tablut.annotation() );
      return false;
    });

    $('.flip', container).click(function() {
      tablut.setUpBoardPieces($('.annot', container).text());
      return false;
    });

    if ( typeof callback != "undefined" ) { callback(tablut) };
  }

//  loadTablutGame( '#game1', { board_size : 7, fen : "3a3/3a3/3p3/aapkpaa/3p3/3a3/3a3", active_side_indicator : $('#indicator1') } );
  loadTablutGame( '#game1', { board_size : 9, active_side_indicator : $('#indicator1') } );

//  loadTablutGame( '#game2', { active_side_indicator : $('#indicator1') } );
//  loadTablutGame( '#game9', { pgn : $('#middle-game').html() }, function(tablut) {
//    tablut.transitionTo(25);
//  });

});
