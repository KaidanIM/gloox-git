%
% (c) 2004-2005 by Jakob Schröter <js@camaya.net>
%

verbatimtex
%&latex
\documentclass{article}
\usepackage{helvet}
\renewcommand*{\familydefault}{\sfdefault}
\begin{document}
etex


% rough client server scenario, including database and statistics viewer
\beginfig(0);
  u:=10;

  draw (15u,15u)--(23u,15u)--(23u,20u)--(15u,20u)--cycle;
  label(btex XMPP Server etex, (19u,17.5u));

  % Workers
  draw (4u,19u)--(10u,19u)--(10u,21u)--(4u,21u)--cycle;
  label(btex Worker etex, (7u,20u));
  drawdblarrow (15u,18.5u)--(10u,20u);

  draw (4u,14u)--(10u,14u)--(10u,16u)--(4u,16u)--cycle;
  label(btex Worker etex, (7u,15u));
  drawdblarrow (15u,16.5u)--(10u,15u);

  % dots between Workers
  pickup pencircle scaled 1.5;
  draw (7u,16.75u);
  draw (7u,17.25u);
  draw (7u,17.75u);
  pickup pencircle scaled 0.5; % restore default pen

  % Feeder
  draw (28u,16.5u)--(34u,16.5u)--(34u,18.5u)--(28u,18.5u)--cycle;
  label(btex Feeder etex, (31u,17.5u));
  drawdblarrow (28u,17.5u)--(23u,17.5u);

  % database
  draw (30u,22u)--(30u,24u);
  draw (32u,22u)--(32u,24u);
  draw halfcircle xscaled 2u yscaled 1.2u rotated 180 shifted (31u, 22u);
  draw fullcircle xscaled 2u yscaled 1.2u shifted (31u, 24u);
  label(btex DB etex, (31u,22.6u));
  drawdblarrow (31u,18.5u)--(31u,21.4u);

  % statistics viewer
  draw (27u,27u)--(35u,27u)--(35u,29u)--(27u,29u)--cycle;
  label(btex Statistics Viewer etex, (31u,28u));
  drawdblarrow (31u,24.6u)--(31u,27u);
\endfig;



% indented with anchors
% n: enumeration value
% l: start week
% h: end week
% holidayFrom: start of holidays
% title: Title
% isSub: is this a sub-item?
% last: is this the last item of a milestone?
def drawObjective(expr n, l, h, holidayFrom, title, isSub, last)=
  if isSub>0:
    label.rt(title, (0+5,(n+1)*u-(u/2)));
    draw (6,(n+1)*u-(u/2))--(3,(n+1)*u-(u/2))--(3,(n+1)*u);
    if last=0:
      draw (3,(n+1)*u-(u/2))--(3,(n)*u);
    fi
  fi
  if isSub<1:
    label.rt(title, (0,(n+1)*u-(u/2)));
  fi
  hLength:=(holiday/u);
  if l>0:
    if l<holidayFrom:
      for k=l-1 upto h-1:
        if k<holidayFrom-1:
          fill (indent+k*u,n*u)--(u+indent+k*u,n*u)--(u+indent+k*u,n*u+u)--(indent+k*u,n*u+u)--cycle withcolor .8white;
          draw (indent+k*u,n*u)--(u+indent+k*u,n*u)--(u+indent+k*u,n*u+u)--(indent+k*u,n*u+u)--cycle withcolor .5white;
        fi
      endfor
    fi
    if h>=holidayFrom:
      for k=l-1+hLength upto h-1+hLength:
        if k>=holidayFrom+hLength-1:
          fill (indent+k*u,n*u)--(u+indent+k*u,n*u)--(u+indent+k*u,n*u+u)--(indent+k*u,n*u+u)--cycle withcolor .8white;
          draw (indent+k*u,n*u)--(u+indent+k*u,n*u)--(u+indent+k*u,n*u+u)--(indent+k*u,n*u+u)--cycle withcolor .5white;
        fi
      endfor
    fi
  fi
enddef;

% thick line with arrows
% n: enumeration value
% l: start week
% h: end week
% holidayFrom: start of holidays
% title: Title
% arBegin: draw arrow at the beginning?
% arEnd: draw arrow at the end?
def drawMilestone(expr n, l, h, holidayFrom, title, arBegin, arEnd)=
  label.rt(title, (0,(n+1)*u-(u/2)));
  hLength:=(holiday/u);
  if l>0:
    pickup pensquare scaled 2;
    if l<holidayFrom:
      for k=l-1 upto h-1:
        if k<holidayFrom-1:
          draw (1+indent+k*u,n*u+(u/2))--(u+indent+k*u-1,n*u+(u/2));
        fi
      endfor
    fi
    if h>=holidayFrom:
      for k=l-1+hLength upto h-1+hLength:
        if k>=holidayFrom+hLength-1:
          draw (1+indent+k*u,n*u+(u/2))--(u+indent+k*u-1,n*u+(u/2));
        fi
      endfor
    fi
    pickup pencircle scaled 1.5;
    %fill (indent+l*u)
    if arBegin>0:
      if l<holidayFrom:
        drawarrow (indent+(l-1)*u,n*u+(u/2))--(indent+(l-1)*u,(n*u+(u/2))-5);
      fi
      if l>=holidayFrom:
        drawarrow (indent+(l-1+hLength)*u,n*u+(u/2))--(indent+(l-1+hLength)*u,(n*u+(u/2))-5);
      fi
    fi
    if arEnd>0:
      if h<holidayFrom:
        drawarrow (u+indent+(h-1)*u,n*u+(u/2))--(u+indent+(h-1)*u,(n*u+(u/2))-5);
      fi
      if h>=holidayFrom:
        drawarrow (u+indent+(h-1+hLength)*u,n*u+(u/2))--(u+indent+(h-1+hLength)*u,(n*u+(u/2))-5);
      fi
    fi
    pickup pencircle scaled .5;
  fi
enddef;

% wi: width in pixel (number of columns x width of a cell)
% he: height in pixel (number of rows x height of a cell)
% ind: indentation of 
% holTime: hwo long are the holidays
% holText: How are the holidays called?
% holAt: When are the holidays?
% capText: Title of the grid
def drawGrid(expr wi, he, ind, holTime, holText, holAt, capText)=
  % --- Grid ---
  width:=wi*u+indent;
  height:=he*u;
  holWidth:=holTime*u;
  % horizontal
  for i=0 upto he:
    draw (0, i*u)--(width+holWidth, i*u) withcolor .5white;
  endfor
  draw (indent, he*u+u)--(width+holWidth, he*u+u) withcolor .5white;
  draw (indent, he*u+2u)--(width+holWidth, he*u+2u) withcolor .5white;
  % vertical
  draw (0, 0)--(0, height) withcolor .5white;
  for j=0 upto wi-(wi-holAt+1):
    draw (j*u+indent, 0)--(j*u+indent, height+u) withcolor .5white;
    if j<wi-(wi-holAt+1):
      label.lft(decimal (j+1), (j*u+indent+u,he*u+5));
    fi
  endfor
  label(holText, ((wi-(wi-holAt+1))*u+indent+(holWidth/2),he*u+5));
  for j=wi-(wi-holAt+1) upto wi:
    draw (j*u+indent+holWidth, 0)--(j*u+indent+holWidth, height+u) withcolor .5white;
    if j<wi:
      label.lft(decimal (j+1), (j*u+indent+holWidth+u,he*u+5));
    fi
  endfor
  draw (indent, height)--(indent, height+2u) withcolor .5white;
  draw (width+holWidth, height)--(width+holWidth, height+2u) withcolor .5white;
  % --- End Grid ---

  label.rt(capText, (indent,he*u+u+(u/2)));
enddef;



% gantt chart
he:=20;
wi:=15;
u:=15;
indent:=90;
width:=wi*u+indent;
height:=he*u;
holiday:=60; % should be mutiple of u


% initial planning, semester 1
beginfig(1)
  drawGrid(wi, he, indent, 4, btex Christmas etex, 13, btex \textbf{Semester 1} $\cdot$ Week etex);

  drawObjective(19,  1,  7, 13, btex Interim Report etex,    0, 0);
  drawObjective(18,  5,  6, 13, btex Requirements etex,      0, 0);
  drawMilestone(17,  7, 12, 13, btex Design etex,            1, 1);
  drawObjective(16,  7, 12, 13, btex Worker etex,            1, 0);
  drawObjective(15,  7, 12, 13, btex Feeder etex,            1, 0);
  drawObjective(14, 11, 12, 13, btex Sample Apps etex,       1, 0);
  drawObjective(13,  7,  7, 13, btex Database etex,          1, 0);
  drawObjective(12, 11, 12, 13, btex Statistics Viewer etex, 1, 0);
  drawObjective(11, 11, 12, 13, btex Feeder Frontend etex,   1, 1);
  drawMilestone(10, 13, 15, 13, btex Implementation etex,    1, 0);
  drawObjective( 9, 13, 15, 13, btex Client etex,            1, 0);
  drawObjective( 8, 13, 15, 13, btex Feeder etex,            1, 0);
  drawObjective( 7, 13, 15, 13, btex Sample Apps etex,       1, 0);
  drawObjective( 6, 13, 13, 13, btex Database etex,          1, 0);
  drawObjective( 5, 15, 15, 13, btex Statistis Viewer etex,  1, 0);
  drawObjective( 4,  0,  0, 13, btex Feeder Frontend etex,   1, 1);
  drawMilestone( 3, 11, 15, 13, btex Documentation etex,     1, 0);
  drawObjective( 2,  0,  0, 13, btex API Doc etex,           1, 0);
  drawObjective( 1, 11, 15, 13, btex User Manuals etex,      1, 1);
  drawObjective( 0,  8, 15, 13, btex Final Report etex,      0, 0);

endfig;


% initial planning, semester 2
beginfig(2)
  drawGrid(wi, he, indent, 4, btex Easter etex, 8, btex \textbf{Semester 2} $\cdot$ Week etex);

  drawObjective(19,  0,  0, 8, btex Interim Report etex,    0, 0);
  drawObjective(18,  0,  0, 8, btex Requirements etex,      0, 0);
  drawMilestone(17,  0,  0, 8, btex Design etex,            0, 0);
  drawObjective(16,  0,  0, 8, btex Worker etex,            1, 0);
  drawObjective(15,  7,  0, 8, btex Feeder etex,            1, 0);
  drawObjective(14,  0,  0, 8, btex Sample Apps etex,       1, 0);
  drawObjective(13,  0,  0, 8, btex Database etex,          1, 0);
  drawObjective(12,  0,  0, 8, btex Statistics Viewer etex, 1, 0);
  drawObjective(11,  0,  0, 8, btex Feeder Frontend etex,   1, 1);
  drawMilestone(10,  1,  6, 8, btex Implementation etex,    0, 1);
  drawObjective( 9,  1,  4, 8, btex Client etex,            1, 0);
  drawObjective( 8,  1,  4, 8, btex Feeder etex,            1, 0);
  drawObjective( 7,  1,  6, 8, btex Sample Apps etex,       1, 0);
  drawObjective( 6,  0,  0, 8, btex Database etex,          1, 0);
  drawObjective( 5,  1,  2, 8, btex Statistis Viewer etex,  1, 0);
  drawObjective( 4,  2,  4, 8, btex Feeder Frontend etex,   1, 1);
  drawMilestone( 3,  1,  7, 8, btex Documentation etex,     0, 1);
  drawObjective( 2,  1,  5, 8, btex API Doc etex,           1, 0);
  drawObjective( 1,  1,  7, 8, btex User Manuals etex,      1, 1);
  drawObjective( 0,  1,  9, 8, btex Final Report etex,      0, 0);
endfig;


%%
% Revised Planning
%%

% gantt chart
he:=22;
wi:=15;
u:=15;
indent:=90;
width:=wi*u+indent;
height:=he*u;
holiday:=60; % should be mutiple of u

% revised planning, semester 1
beginfig(3)
  drawGrid(wi, he, indent, 4, btex Christmas etex, 13, btex \textbf{Semester 1} $\cdot$ Week etex);

  drawObjective(21,  1,  7, 13, btex Interim Report etex,    0, 0);
  drawObjective(20,  5,  6, 13, btex Requirements etex,      0, 0);
  drawMilestone(19,  7, 15, 13, btex Design etex,            1, 0);
  drawObjective(18,  8, 15, 13, btex XMPP Lib etex,          1, 0);
  drawObjective(17, 15, 15, 13, btex Worker etex,            1, 0);
  drawObjective(16, 15, 15, 13, btex Feeder etex,            1, 0);
  drawObjective(15,  0,  0, 13, btex Sample Apps etex,       1, 0);
  drawObjective(14,  7,  7, 13, btex Database etex,          1, 0);
  drawObjective(13,  0,  0, 13, btex Statistics Viewer etex, 1, 0);
  drawObjective(12,  0,  0, 13, btex Feeder Frontend etex,   1, 1);
  drawMilestone(11,  8, 15, 13, btex Implementation etex,    1, 0);
  drawObjective(10,  8, 15, 13, btex XMPP Lib etex,          1, 0);
  drawObjective( 9,  0,  0, 13, btex Worker etex,            1, 0);
  drawObjective( 8,  0,  0, 13, btex Feeder etex,            1, 0);
  drawObjective( 7,  0,  0, 13, btex Sample Apps etex,       1, 0);
  drawObjective( 6,  0,  0, 13, btex Database etex,          1, 0);
  drawObjective( 5,  0,  0, 13, btex Statistis Viewer etex,  1, 0);
  drawObjective( 4,  0,  0, 13, btex Feeder Frontend etex,   1, 1);
  drawMilestone( 3,  0,  0, 13, btex Documentation etex,     1, 0);
  drawObjective( 2,  0,  0, 13, btex API Doc etex,           1, 0);
  drawObjective( 1,  0,  0, 13, btex User Manuals etex,      1, 1);
  drawObjective( 0,  0,  0, 13, btex Final Report etex,      0, 0);

endfig;


% revised planning, semester 2
beginfig(4)
  drawGrid(wi, he, indent, 4, btex Easter etex, 8, btex \textbf{Semester 2} $\cdot$ Week etex);

  drawObjective(21,  0,  0, 8, btex Interim Report etex,    0, 0);
  drawObjective(20,  0,  0, 8, btex Requirements etex,      0, 0);
  drawMilestone(19,  1,  6, 8, btex Design etex,            0, 1);
  drawObjective(18,  1,  3, 8, btex XMPP Lib etex,          1, 0);
  drawObjective(17,  1,  4, 8, btex Worker etex,            1, 0);
  drawObjective(16,  1,  4, 8, btex Feeder etex,            1, 0);
  drawObjective(15,  4,  5, 8, btex Sample Apps etex,       1, 0);
  drawObjective(14,  0,  0, 8, btex Database etex,          1, 0);
  drawObjective(13,  6,  6, 8, btex Statistics Viewer etex, 1, 0);
  drawObjective(12,  6,  6, 8, btex Feeder Frontend etex,   1, 1);
  drawMilestone(11,  1,  7, 8, btex Implementation etex,    0, 1);
  drawObjective(10,  1,  4, 8, btex XMPP Lib etex,          1, 0);
  drawObjective( 9,  3,  5, 8, btex Worker etex,            1, 0);
  drawObjective( 8,  3,  5, 8, btex Feeder etex,            1, 0);
  drawObjective( 7,  4,  7, 8, btex Sample Apps etex,       1, 0);
  drawObjective( 6,  5,  5, 8, btex Database etex,          1, 0);
  drawObjective( 5,  6,  6, 8, btex Statistis Viewer etex,  1, 0);
  drawObjective( 4,  7,  7, 8, btex Feeder Frontend etex,   1, 1);
  drawMilestone( 3,  2,  7, 8, btex Documentation etex,     1, 1);
  drawObjective( 2,  2,  5, 8, btex API Doc etex,           1, 0);
  drawObjective( 1,  3,  7, 8, btex User Manuals etex,      1, 1);
  drawObjective( 0,  1,  9, 8, btex Final Report etex,      0, 0);
endfig;



%%
% Final Planning
%%

% gantt chart
he:=22;
wi:=15;
u:=15;
indent:=90;
width:=wi*u+indent;
height:=he*u;
holiday:=60; % should be mutiple of u

% final planning, semester 1
beginfig(5)
  drawGrid(wi, he, indent, 4, btex Christmas etex, 13, btex \textbf{Semester 1} $\cdot$ Week etex);

  drawObjective(21,  1,  7, 13, btex Interim Report etex,    0, 0);
  drawObjective(20,  5,  6, 13, btex Requirements etex,      0, 0);
  drawMilestone(19,  7, 15, 13, btex Design etex,            1, 0);
  drawObjective(18,  8, 15, 13, btex XMPP Lib etex,          1, 0);
  drawObjective(17, 15, 15, 13, btex Worker etex,            1, 0);
  drawObjective(16, 15, 15, 13, btex Feeder etex,            1, 0);
  drawObjective(15,  0,  0, 13, btex Sample Apps etex,       1, 0);
  drawObjective(14,  7,  7, 13, btex Database etex,          1, 0);
  drawObjective(13,  0,  0, 13, btex Statistics Viewer etex, 1, 0);
  drawObjective(12,  0,  0, 13, btex Feeder Frontend etex,   1, 1);
  drawMilestone(11,  8, 15, 13, btex Implementation etex,    1, 0);
  drawObjective(10,  8, 15, 13, btex XMPP Lib etex,          1, 0);
  drawObjective( 9,  0,  0, 13, btex Worker etex,            1, 0);
  drawObjective( 8,  0,  0, 13, btex Feeder etex,            1, 0);
  drawObjective( 7,  0,  0, 13, btex Sample Apps etex,       1, 0);
  drawObjective( 6,  0,  0, 13, btex Database etex,          1, 0);
  drawObjective( 5,  0,  0, 13, btex Statistis Viewer etex,  1, 0);
  drawObjective( 4,  0,  0, 13, btex Feeder Frontend etex,   1, 1);
  drawMilestone( 3,  0,  0, 13, btex Documentation etex,     1, 0);
  drawObjective( 2,  0,  0, 13, btex API Doc etex,           1, 0);
  drawObjective( 1,  0,  0, 13, btex User Manuals etex,      1, 1);
  drawObjective( 0,  0,  0, 13, btex Final Report etex,      0, 0);

endfig;


% final planning, semester 2
beginfig(6)
  drawGrid(wi, he, indent, 4, btex Easter etex, 8, btex \textbf{Semester 2} $\cdot$ Week etex);

  drawObjective(21,  0,  0, 8, btex Interim Report etex,    0, 0);
  drawObjective(20,  0,  0, 8, btex Requirements etex,      0, 0);
  drawMilestone(19,  1,  6, 8, btex Design etex,            0, 1);
  drawObjective(18,  1,  3, 8, btex XMPP Lib etex,          1, 0);
  drawObjective(17,  1,  4, 8, btex Worker etex,            1, 0);
  drawObjective(16,  1,  4, 8, btex Feeder etex,            1, 0);
  drawObjective(15,  4,  5, 8, btex Sample Apps etex,       1, 0);
  drawObjective(14,  0,  0, 8, btex Database etex,          1, 0);
  drawObjective(13,  6,  6, 8, btex Statistics Viewer etex, 1, 0);
  drawObjective(12,  6,  6, 8, btex Feeder Frontend etex,   1, 1);
  drawMilestone(11,  1,  7, 8, btex Implementation etex,    0, 1);
  drawObjective(10,  1,  4, 8, btex XMPP Lib etex,          1, 0);
  drawObjective( 9,  3,  5, 8, btex Worker etex,            1, 0);
  drawObjective( 8,  3,  5, 8, btex Feeder etex,            1, 0);
  drawObjective( 7,  4,  7, 8, btex Sample Apps etex,       1, 0);
  drawObjective( 6,  5,  5, 8, btex Database etex,          1, 0);
  drawObjective( 5,  6,  6, 8, btex Statistis Viewer etex,  1, 0);
  drawObjective( 4,  7,  7, 8, btex Feeder Frontend etex,   1, 1);
  drawMilestone( 3,  2,  7, 8, btex Documentation etex,     1, 1);
  drawObjective( 2,  2,  5, 8, btex API Doc etex,           1, 0);
  drawObjective( 1,  3,  7, 8, btex User Manuals etex,      1, 1);
  drawObjective( 0,  1,  9, 8, btex Final Report etex,      0, 0);
endfig;


% sequence diagram showing observer example
beginfig(7)
  he:=200;
  bh:=25;
  start:=0;
  pf:=150;
  le:=100;
  % squares                                                            % labels
  draw (pf*0,he)--(pf*0+le,he)--(pf*0+le,he+bh)--(pf*0,he+bh)--cycle;  label (btex Feeder etex,      (pf*0+le/2,he+bh/2));
  draw (pf*1,he)--(pf*1+le,he)--(pf*1+le,he+bh)--(pf*1,he+bh)--cycle;  label (btex Client Lib etex,  (pf*1+le/2,he+bh/2));
  draw (pf*2,he)--(pf*2+le,he)--(pf*2+le,he+bh)--(pf*2,he+bh)--cycle;  label (btex XMPP Server etex, (pf*2+le/2,he+bh/2));

  % lifelines (vertical dashed)
  % upper half                                              % lower half
  draw (pf*0+le/2,he)--(pf*0+le/2,he/2+5) dashed evenly;    draw (pf*0+le/2,he/2-5)--(pf*0+le/2,0) dashed evenly;
  draw (pf*1+le/2,he)--(pf*1+le/2,he/2+5) dashed evenly;    draw (pf*1+le/2,he/2-5)--(pf*1+le/2,0) dashed evenly;
  draw (pf*2+le/2,he)--(pf*2+le/2,he/2+5) dashed evenly;    draw (pf*2+le/2,he/2-5)--(pf*2+le/2,0) dashed evenly;

  % "time-goes-by indicators"
  % upper half                                              % lower half
  draw ((pf*0+le/2)-5,he/2-5-2)--((pf*0+le/2)+5,he/2-5+2);  draw ((pf*0+le/2)-5,he/2+5-2)--((pf*0+le/2)+5,he/2+5+2);
  draw ((pf*1+le/2)-5,he/2-5-2)--((pf*1+le/2)+5,he/2-5+2);  draw ((pf*1+le/2)-5,he/2+5-2)--((pf*1+le/2)+5,he/2+5+2);
  draw ((pf*2+le/2)-5,he/2-5-2)--((pf*2+le/2)+5,he/2-5+2);  draw ((pf*2+le/2)-5,he/2+5-2)--((pf*2+le/2)+5,he/2+5+2);


  % function calls                             % labels
  drawarrow (pf*0+le/2,150)--(pf*1+le/2,150);  label (btex registerMessageHandler() etex,(((pf*1+le/2)-(pf*0+le/2))/2+(pf*0+le/2),150+5));
  %drawarrow (pf*1+le/2,60)--(pd*2+le/2,60);
  drawarrow (pf*2+le/2,70)--(pf*1+le/2,70);    label (btex incoming XMPP message etex, (((pf*2+le/2)-(pf*1+le/2))/2+(pf*1+le/2),70+5));
  drawarrow (pf*1+le/2,50)--(pf*0+le/2,50);    label (btex handleMessage() etex,       (((pf*1+le/2)-(pf*0+le/2))/2+(pf*0+le/2),50+5));

endfig;

u:=10;
beginfig(8)
  % Worker
  draw (8u,15u)--(14u,15u)--(14u,18u)--(8u,18u)--cycle;
  label(btex Worker etex, (11u,16.5u));
  drawarrow (11u,18u)--(17.5u,25u);

  % Feeder
  draw (26u,15u)--(32u,15u)--(32u,18u)--(26u,18u)--cycle;
  label(btex Feeder etex, (29u,16.5u));
  drawarrow (29u,18u)--(22.5u,25u);

  % XMPP Client Lib
  draw (15u,25u)--(25u,25u)--(25u,28u)--(15u,28u)--cycle;
  label(btex XMPP Client Lib etex, (20u,26.5u));

endfig;
end
