var gstrUserId = "";
var gstrUserPasswd = "";
var gstrToId = "";
var gstrSdp = "";
var ws = null;

btnRetreiveList.disabled = true;
btnInvite.disabled = true;
btnAccept.disabled = true;
btnDecline.disabled = true;
btnBye.disabled = true;

function CheckDuplicateId()
{
  Log("Check Duplicate ID");
}

function SendUserInfo()
{
  let UserId = document.getElementById('userid_id');
  let UserPasswd = document.getElementById('password_id');
  let UserName = document.getElementById('name_id');
  let UserEmail = document.getElementById('email_id');
  let UserPhone = document.getElementById('phone_id');
  let UserAddress = document.getElementById('address_id');

  let emailRules = /^[A-Za-z0-9_\.\-]+@[A-Za-z0-9\-]+\.[A-Za-z0-9\-]+/;
  let passwordRules = /^(?=.*[a-zA-Z])(?=.*[!@#$%^*+=-])(?=.*[0-9]).{10,15}$/;
  let websocket = null;

  if( UserEmail.value.length == 0 )
  {
    Log( "Email has not been entered" );
    alert( "Email has not been entered" );
    return;
  }

  if(emailRules.test(UserEmail.value)==false){
    //이메일 형식이 알파벳+숫자@알파벳+숫자.알파벳+숫자 형식이 아닐경우
    Log( "The email format is not valid.: "+ UserEmail.value );
    alert("The email format is not valid.");
    return;
  }

  if(passwordRules.test(UserPasswd.value)==false){
    Log( "The Password format is not valid.: "+ UserPasswd.value );
    alert("The Password format is not valid.");
    return;
  }

  if( websocket == null )
  {
    if( window.location.protocol == "https:" )
    {
      websocket = new WebSocket("wss://" + window.location.hostname );
    }
    else
    {
      websocket = new WebSocket("ws://" + window.location.hostname + ":8080");
    }

    websocket.onopen = function(e){
      websocket.send("req|register|" +
                      UserId.value +"|" +
                      UserPasswd.value + "|" +
                      UserName.value + "|" +
                      UserEmail.value + "|" +
                      UserPhone.value + "|" +
                      UserAddress.value)
    };
    // websocket 에서 수신한 메시지를 화면에 출력한다.
    websocket.onmessage = function(e){
      Log("Recv[" + e.data + "]");
	
      var arrData = e.data.split("|");

      switch( arrData[0] )
      {
        case "res":
          switch( arrData[1] )
          {
            case "register":
              if( arrData[2] == '200' )
              {
                alert("User registration was successful.["+ UserId.value +"]");
              }
              else
              {
                var iStatusCode = parseInt( arrData[2] );

                if (iStatusCode == 400)
                {
                  alert("user ID is alread registered\r\nPlease use another user ID");
                }
                else if (iStatusCode == 410)
                {
                  alert("can not INSERT the user info to mysql");
                }
              }
              break;
          }
          break;
      }
    };	
    // websocket 세션이 종료되면 화면에 출력한다.
    websocket.onclose = function(e){
    websocket = null;
    Log("WebSocket is closed");
    }
  }
}

function StartSession()
{
  var txtUserId = document.getElementById('user_id');
  gstrUserId = txtUserId.value;

  var txtUserPasswd = document.getElementById('user_passwd');
  gstrUserPasswd = txtUserPasswd.value;
  
  if( gstrUserId.length == 0 )
  {
    alert( "user id is not defined" );
    return;
  }
  if( gstrUserPasswd.length == 0 )
  {
    alert( "password is not provided" );
    return;
  }
  
  if( ws == null )
  {
  	if( window.location.protocol == "https:" )
    {
      ws = new WebSocket("wss://" + window.location.hostname );
    }
    else
    {
      ws = new WebSocket("ws://" + window.location.hostname + ":8080");
    }

    // websocket 서버에 연결되면 연결 메시지를 화면에 출력한다.
    ws.onopen = function(e){
        Send( "req|login|" + gstrUserId + "|" + gstrUserPasswd );
    };

    // websocket 에서 수신한 메시지를 화면에 출력한다.
    ws.onmessage = function(e){
		
    Log("Recv[" + e.data + "]");

    var arrData = e.data.split("|");

    switch( arrData[0] )
    {
      case "res":
        switch( arrData[1] )
        {
          case "login":
            if( arrData[2] == '200' )
            {
              btnLogin.disabled = true;
              btnRetreiveList.disabled = false;
              btnInvite.disabled = false;
            }
            else
            {
              var iStatusCode = parseInt( arrData[2] );

              if (iStatusCode == 300)
              {
                  alert("Unregisterd user! \r\nPlease register to use VideoChat");
              }
              else if (iStatusCode == 400)
              {
                  alert("Password is WRONG!");
              }
              else if (iStatusCode == 410)
              {
                  alert("Password Database failed");
              }
              else if (iStatusCode == 420)
              {
                  alert("Too many password is wrong. \r\n Wait for 60 seconds...");
              }
              btnLogin.disabled = false;
              btnInvite.disabled = true;
            }
            break;
          case "invite":
            if( arrData[2] == '200' )
            {
              setAnswer( arrData[3] );

              btnInvite.disabled = true;
              btnAccept.disabled = true;
              btnDecline.disabled = true;
              btnBye.disabled = false;
            }
            else
            {
              var iStatusCode = parseInt( arrData[2] );

              if( iStatusCode == 404 )
              {
                  alert("peer id is NOT log-in now!\r\nPlease press \"Retreive logined ID\" button");
                  btnInvite.disabled = false;
              }
              else if( iStatusCode == 500 )
              {
                  alert("peer id is ALREADY in the another call\r\nYou can connect after previous call is finished!");
                  btnInvite.disabled = false;
              }
              else if( iStatusCode > 200 )
              {
                btnInvite.disabled = false;
              }
            }
            btnBye.disabled = false;
            break;
          case "contact":
            ClearLog()
            Print("### Contact ID Logined ###")
			//Print("unique_id   /  name  /   email  /   phone  / address ")
            var iContactCount = parseInt( arrData[2] );
            for (var i = 0; i < iContactCount; i++)
            {
              //Print(arrData[3+i])
              // send userinfo
              Send( "req|userinfo|" + arrData[3+i] );
            }
            break;
          case "userinfo":
			Print("[" + arrData[2] + "]  " + arrData[3] + " / " + arrData[4] + " / " + arrData[5] + " / " + arrData[6])
            break;
        }
        break;

      case "req":
        switch( arrData[1] )
        {
          case "invite":
            gstrToId = arrData[2];
            gstrSdp = arrData[3];
            var txtPeerId = document.getElementById('peer_id');
            txtPeerId.value = gstrToId;

            Log("Invite event peer(" + gstrToId + ") sdp(" + arrData[3]+ ")" );
            alert( 'You have a call from ' + gstrToId + '\r\nPlease press Accept or Decline!' );
            //createAnswer( clsData.sdp );

            btnInvite.disabled = true;
            btnAccept.disabled = false;
            btnDecline.disabled = false;
            btnBye.disabled = true;
            break;
          case "bye":
            if (btnAccept.disabled == false)
                alert( 'You have a missedcall from ' + gstrToId);
            else
                alert( gstrToId + 'call ended');
            gstrToId = "";
            stopPeer();
            btnInvite.disabled = false;
            btnAccept.disabled = true;
            btnDecline.disabled = true;
            btnBye.disabled = true;
            break;
        }
      }
    };

    // websocket 세션이 종료되면 화면에 출력한다.
    ws.onclose = function(e){
    ws = null;
    InitButton();
    Log("WebSocket is closed");
    }
  }
}

function ContactList()
{
  Send( "req|contact" );
}

function Send(data)
{
	ws.send(data);
	Log( "Send[" + data + "]" );
}

function Invite(strSdp)
{
	Send( "req|invite|" + gstrToId + "|" + strSdp );
}

/** 전화 수신한다. */
function Accept(strSdp)
{
	Send( "res|invite|200|" + strSdp );
}

function SendInvite()
{
  var txtPeerId = document.getElementById('peer_id');
  var strPeerId = txtPeerId.value;

  if( strPeerId.length == 0 )
  {
    alert( 'peer id is not defined' );
    return;
  }

  btnInvite.disabled = true;
  btnAccept.disabled = true;
  btnDecline.disabled = true;
  btnBye.disabled = true;

  gstrToId = strPeerId;
  createOffer();

  //Invite( "o=" + strPeerId );
}

function SendAccept()
{
  btnInvite.disabled = true;
  btnAccept.disabled = true;
  btnDecline.disabled = true;
  btnBye.disabled = false;

  createAnswer(gstrSdp);
  //Accept( "o=accept" );
}

/** INVITE 거절 응답을 전송한다. */
function SendDecline()
{
  Send( "res|invite|603" );
	
  btnInvite.disabled = false;
  btnAccept.disabled = true;
  btnDecline.disabled = true;
}

/** BYE 를 전송한다. */
function SendBye()
{
  stopPeer();
  Send( "req|bye" );
  gstrToId = "";
  btnInvite.disabled = false;
  btnBye.disabled = true;
}
