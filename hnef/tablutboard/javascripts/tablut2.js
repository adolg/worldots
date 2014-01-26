// If Firebug is not installed, prevent console.log() from creating error messages
if (typeof console == "undefined") { var console = { log: function() {} }; }

// Iterate within an arbitrary context...
jQuery.eachWithContext = function(context, object, callback) {
  for ( var i = 0, length = object.length, value = object[0];
    i < length && callback.call(context, i, value ) !== false; value = object[++i] ) {}
};

(function($) {
  /* Constructor */
  $.tablut = function(options, wrapper) {
    this.settings = $.extend( {}, $.tablut.defaults, options );
    this.wrapper = wrapper;

    this.game = {

      move_origin : { top : 0, left : 0 },
      halfmove_clock : 0,

      transition_current : 0,

      header : [],
      body : '',
      captured_stack : [100,200,0,300,1000,1000,1000,1000],
      annotations : [],
      raw_annotations : [],

      next_piece_id : 64,
      transition : {},
      transitions : [],
      board_direction : 1,
      board_size : 9
    };

  };

/* Add tablut() to the jQuery namespace */
  $.fn.tablut = function(options) {
    var tablut = new $.tablut(options, this[0]);
    tablut.init();
    return tablut;
  };

  $.extend($.tablut, {

    defaults : {
      square_size : 70,
      offsets : { left: 0, top: 0 },
      board_element_selector : '.tablut-board',
      json_annotations : false,
      board_size : 9,
      rules : 'tablut0'
    },

    prototype : {
      init : function() {
        // Load a fresh board position
        //this.setUpBoardPieces( '000222000000020000000010000000010000' );
        this.setUpBoardSquares( '222888222211181112211111112811111118' );
        // If pgn was passed in, parse it
        //if (this.settings.pgn) this.parsePGN(this.settings.pgn);

        //this.setUpBoard( this.parseFEN( this.settings.fen ) );
      },

      transitionBackward : function() {
        if (this.game.transition_current == 0)
		return;
	this.game.transition_current = this.game.transition_current - 1;
	var transToRollBack = this.game.transitions[this.game.transition_current];
	// un-fade pieces still present on the board
	if (transToRollBack.winner) {
        $.eachWithContext(this, this.boardData(), function(j, row) {
          $.eachWithContext(this, row, function(k, piece) {
            if (piece != '-') {
               var node = $('#' + this.getDomPieceId(piece.id));
               node.show('fade');
            }
          });
        });
        }

	if (transToRollBack.captured_pieces)
	// restore all captured pieces
        $.eachWithContext(this, transToRollBack.captured_pieces, function(j, capture) {
          var node = $('#' + this.getDomPieceId(capture.piece.id));
          node.show('fade');
	  // update board model
	  this._board[capture.top][capture.left] = capture.piece;
	});
	// move the piece that was last to move, back
	var mover = this._board[transToRollBack.target.top][transToRollBack.target.left];
	// update model
        this._board[transToRollBack.target.top][transToRollBack.target.left] = '-';
        this._board[transToRollBack.origin.top][transToRollBack.origin.left] = mover;
	// move piece visually
        var pos_top = this.settings.square_size * transToRollBack.origin.top + this.settings.offsets.top;
        var pos_left = this.settings.square_size * transToRollBack.origin.left + this.settings.offsets.left;

        var node = $('#' + this.getDomPieceId(mover.id));
        node.animate({ position: 'absolute', top:pos_top, left:pos_left });
        this.showActiveSide();
      },

      transitionForward : function() {
	console.log('transition_current = ' + this.game.transition_current);
        if (this.game.transition_current >= this.game.transitions.length)
		return;

	var transToReplay = this.game.transitions[this.game.transition_current];

	// move the piece that was moving, again
	var mover = this._board[transToReplay.origin.top][transToReplay.origin.left];
	// update model
        this._board[transToReplay.origin.top][transToReplay.origin.left] = '-';
        this._board[transToReplay.target.top][transToReplay.target.left] = mover;
	// move piece visually
        var pos_top = this.settings.square_size * transToReplay.target.top + this.settings.offsets.top;
        var pos_left = this.settings.square_size * transToReplay.target.left + this.settings.offsets.left;

        var node = $('#' + this.getDomPieceId(mover.id));
        node.animate({ position: 'absolute', top:pos_top, left:pos_left });

	if (transToReplay.captured_pieces)
	// fade all captured pieces
        $.eachWithContext(this, transToReplay.captured_pieces, function(j, capture) {
          var node = $('#' + this.getDomPieceId(capture.piece.id));
          node.effect('fade');
	  // update board model
	  this._board[capture.top][capture.left] = '-';
	});
	if (transToReplay.winner) {
           if (transToReplay.winner == 'w') {
              $('#' + this.wrapper.id +' .piecea').effect('fade');
           } else {
              $('#' + this.wrapper.id +' .piecep').effect('fade');
           }
        }
	this.game.transition_current = this.game.transition_current + 1;
        this.showActiveSide();
      },

      annotation : function() {
      },

      boardElement : function() {
        return $(this.wrapper).find(this.settings.board_element_selector);
      },

      boardData : function() {
        return this._board;
      },

      setUpBoardPieces : function(template) {
    	  if (!this._board)
    	  {
	        this._board = this.createBoardDataFromTemplate(template);
	        this.writeBoard();
    	  } else {
    		  this.boardRepaint(template);
    	  }
        //this.game.active_color = this.settings.rules.first_move;
        this.showActiveSide();
      },

      setUpBoardSquares : function(template) {
        this._squares = this.createBoardDataFromTemplate(template);
      },

      createBoardDataFromTemplate : function(template) {
        var board = [];
        for (var i = 0; i < template.length; i++)
	{
	  board[i] = Number(template.charAt(i));
	}
        return board;
      },

      activeSide : function() {
        if (this.game.transition_current % 2) {
            //if (this.settings.rules.first_move = 'w')
                return 'b';
            //else
                //return 'w';
        } else
        	return 'w';
            //return this.settings.rules.first_move;
      },

      showActiveSide : function() {
        var indicator = this.settings.active_side_indicator;
        if (indicator) {
          if (this.activeSide() == 'w') {
            indicator.removeClass('piecea');
            indicator.addClass('piecep');
          } else {
            indicator.removeClass('piecep');
            indicator.addClass('piecea');
          }
        }
      },

      writeBoard : function() {
        var board = this.boardElement();
        if (board.size() == 0) {
          $(this.wrapper).append('<div class="tablut-board"></div>');
          board = this.boardElement();
        }

        // board rulers
        for (var j = 0; j < this.settings.board_size; j++) {
          var first = (j == 0 ? " first" : "");
          board.append('<div id="' + this.getRulerId(j,"h") + '" class="horizontal' + first + '"></div>');
          $('#' + this.getRulerId(j,"h")).css({ position: 'absolute', top:this.settings.square_size*j, left:0, width: this.settings.square_size * this.settings.board_size + 'px' });
          board.append('<div id="' + this.getRulerId(j,"v") + '" class="vertical' + first + '"></div>');
          $('#' + this.getRulerId(j,"v")).css({ position: 'absolute', top:0, left:this.settings.square_size*j, height: this.settings.square_size * this.settings.board_size + 'px' });
        }

	// put pieces
	var pieceId = 0;
        $.eachWithContext(this, this.boardData(), function(j, row) {
 
            if (row) this.addDomPiece(row, pieceId++, [~~(j / this.settings.board_size), j % this.settings.board_size]);
 
        });
	// put special squares
	var squareId = 0;
        $.eachWithContext(this, this._squares, function(j, row) {
 
            if (row > 1) this.addSpecialSquare(row, squareId++, [~~(j / this.settings.board_size), j % this.settings.board_size]);
 
        });
        board.droppable({ drop : this.draggingStopped });
        var size = this.settings.board_size*this.settings.square_size;
        board.css({ width:size+'px', height:size+'px' });
      },

      addSpecialSquare : function(type, id, square) {
	  var cellId = this.getSpecialCellId(id, type);
          this.boardElement().append('<div id="' + cellId + '" class="square' + type + '"></div>');
          $('#' + cellId).css({ position: 'absolute', top:square[0]*this.settings.square_size, left:square[1]*this.settings.square_size });
      },
        
      getDomPieceId : function(id) {
        return this.wrapper.id + "_piece_" + id;
      },

      getRulerId : function(id, dir) {
        return this.wrapper.id + "_" + dir + "ruler_" + id;
      },

      getSpecialCellId : function(id, type) {
        return this.wrapper.id + "_s" + type + "_" + id;
      },

      addDomPiece : function(piece, id, square) {
        var pos_top = this.settings.square_size * square[0] + this.settings.offsets.top;
        var pos_left = this.settings.square_size * square[1] + this.settings.offsets.left;
	var domID = this.getDomPieceId(square[0]*this.settings.board_size + square[1]);
        this.boardElement().append('<div id="' + domID + '" class="piece' + piece + '"></div>');
        var node = $('#' + domID);
        node.css({ position: 'absolute', top:pos_top, left:pos_left });
	node.draggable({ grid: [ this.settings.square_size, this.settings.square_size ], 
		containment: "parent", opacity: 0.7, helper: "clone",
                start: this.draggingStarted,
		drag: this.draggingInProgress
        });
        node.data('game', this);
      },

      draggingStarted : function(event, ui) {
        var game = $(this).data('game');
        game.game.move_origin.top = ~~($(this).position().top/game.settings.square_size);
        game.game.move_origin.left = ~~($(this).position().left/game.settings.square_size);  
      },

      draggingInProgress : function(event, ui) {
        var game = $(this).data('game');
        var target = game.screenToModel(ui.position);
        var active = game.belongsToActiveSide($(this));
        if (active && game.objectEquals(target, game.game.move_origin))
          return;
        if (active && game.isLegalMove($(this), ui.position)) {
          ui.helper.css({ border: "none" });
        } else {
          ui.helper.css({ border: "solid red" });
        }
      },

      belongsToActiveSide : function(node) {
        var active = this.activeSide();
        return active == 'b' && node.hasClass('piece2') || active == 'w' && (node.hasClass('piece1') || node.hasClass('piecek'));
      },

      screenToModel : function(pos) {
        return {top:~~(pos.top/this.settings.square_size), left: ~~(pos.left/this.settings.square_size)};
      },

      draggingStopped : function(event, ui) {
        var node = ui.draggable;
        var game = node.data('game');
        //console.log(game.game.active_color);

        if (game.belongsToActiveSide(node)) {
          if (game.isLegalMove(node, ui.position)) {
            node.css({ top:ui.position.top, left:ui.position.left });

            game.completeMove(node, ui.position);
          }
        }
      },

      isLegalMove : function(node, pos) {
        var target = this.screenToModel(pos);
        return this.freeWay(this.game.move_origin, target);
      },

      freeWay : function(origin, target) {
        var piece = this._board[origin.top*this.settings.board_size + origin.left];

        var minTop = Math.min(origin.top,target.top);
        var maxTop = Math.max(origin.top,target.top);
        var minLeft = Math.min(origin.left,target.left);
        var maxLeft = Math.max(origin.left,target.left);
        var piece2 = this._board[target.top*this.settings.board_size + target.left];
 
//	console.log('piece2=' + piece2);
//	console.log('origin=' + origin);
//	console.log('target=' + target);
        if ( piece2 == 0 && (origin.top == target.top || origin.left == target.left) ) {
          for (var j = minTop + 1; j < maxTop; j++) {
            if (this._board[j*this.settings.board_size + origin.left] > 0)
              return false;
          }
          for (var k = minLeft + 1; k < maxLeft; k++) {
            if (this._board[origin.top*this.settings.board_size + k] > 0)
              return false;
          }
          return true;
        }
        return false;
      },

      completeMove : function(node, pos) {
        // update board model
        //var board = this.boardData();
        var piece = this._board[this.game.move_origin.top*this.settings.board_size + this.game.move_origin.left];
        //console.log(piece);
        this._board[this.game.move_origin.top*this.settings.board_size + this.game.move_origin.left] = 0;
        var target = this.screenToModel(pos);
	// update id according to number of new square
        node.attr('id',this.getDomPieceId(target.top*this.settings.board_size + target.left));
        this._board[target.top*this.settings.board_size + target.left] = piece;
	// PGN-like transitions?
	this.game.transition = {origin:this.game.move_origin, target:target};
        //this.debugBoard();
	this.appendTransition();
	// print debug information
        $.eachWithContext(this, this.game.transitions, function(j, move) {
          console.log('[' + j + '] = { origin.top: ' + move.origin.top + ', origin.left: ' + move.origin.left + ' } { target.top: ' + move.target.top + ', target.left: ' + move.target.left + ' }');
	  if (move.captured_pieces)
          $.eachWithContext(this, move.captured_pieces, function(k, val) {
            console.log('[' + j + ',' + k + '] = { top: ' + val.top + ', left: ' + val.left + ' }');
          });
        });
	// TODO send new move to module and process boardRepaint sent back
	//this.boardRepaint( '000222000000020000000010000000001000' );
	//this.boardRepaint( '000202000000020000000010000000011000' );
      },

      boardRepaint : function(template) {
	var modifiedBoard = this.createBoardDataFromTemplate(template);
        $.eachWithContext(this, this.boardData(), function(j, row) {
 	    if (modifiedBoard[j] != row)
	    {
		if(row == 0)
		{
		    // was it the piece that moved or was it rolling back a capture because of undo?
		    var node = $('#' + this.getDomPieceId(j));
		    if (!node.length)
		    {
			node = $('#' + this.getDomPieceId(--this.game.captured_stack[modifiedBoard[j]-1]));
			var pos_top = this.settings.square_size * ~~(j/this.settings.board_size) + this.settings.offsets.top;
			var pos_left = this.settings.square_size * ~~(j % this.settings.board_size) + this.settings.offsets.left;
		        node.animate({ position: 'absolute', top:pos_top, left:pos_left });
			node.show('fade');
			// TODO fade it's copy at certain pos outside, in the POW camp
			node.attr("id",this.getDomPieceId(j));
			this._board[j] = modifiedBoard[j];
		    }
		}
		if(modifiedBoard[j] == 0)
		{
		    // does the square still hold a piece?
		    var node = $('#' + this.getDomPieceId(j));
		    
		    // change piece id to one of captured stack ids
		    node.effect('fade');
		    node.attr("id",this.getDomPieceId(this.game.captured_stack[row-1]++));
		    // TODO unfade it's copy at certain pos outside
		    this._board[j] = 0;
		}
	    }
        });
	//this._board = this.createBoardDataFromTemplate(template);	
      },

      appendTransition : function() {
	// deep copy of current transition
	var newTrans = jQuery.extend(true, {}, this.game.transition);
	this.game.transitions.push(newTrans);
	this.game.transition_current++;
        this.showActiveSide();
      },

      disableOrEnableDrag : function() {
        var white = this.activeSide() == 'w';
	// TODO
        $(".piece2").draggable( "option", "disabled", white );
        $(".piece1").draggable( "option", "disabled", !white );
        $(".piece4").draggable( "option", "disabled", !white );
      },

      debugBoard : function() {
        $.eachWithContext(this, this.boardData(), function(j, row) {
            if (row > 0)
            console.log('[' + j + '] = { piece: ' + row + ' }');
        });
      },

      /* Utility Functions */
      algebraic2Coord : function(algebraic) {
        return [this.rank2Row(algebraic.substr(1, 1)), this.file2Col(algebraic.substr(0, 1))];
      },

      coord2Algebraic : function(row, col) {
        return this.col2File(col) + this.row2Rank(row);
      },

      rank2Row : function(rank) {
        return this.settings.board_size - parseInt(rank);
      },

      file2Col : function(file) {
        return file.charCodeAt(0) - ('a').charCodeAt(0);
      },

      row2Rank : function(row) {
        return (this.settings.board_size - row) + '';
      },

      col2File : function(col) {
        return String.fromCharCode( col + ('a').charCodeAt(0) );
      },

      flipVector : function(v) {
        return { x: (v.x * -1), y : (v.y * -1), limit : v.limit };
      },

      objectEquals : function(obj1, obj2) {
	    for (var i in obj1) {
		if (obj1.hasOwnProperty(i)) {
		    if (!obj2.hasOwnProperty(i)) return false;
		    if (obj1[i] != obj2[i]) return false;
		}
	    }
	    for (var i in obj2) {
		if (obj2.hasOwnProperty(i)) {
		    if (!obj1.hasOwnProperty(i)) return false;
		    if (obj1[i] != obj2[i]) return false;
		}
	    }
	    return true;
      }

    }
  });
})(jQuery);
