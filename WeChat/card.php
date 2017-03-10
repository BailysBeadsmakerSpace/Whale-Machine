<?php
// 作者：Gavin 林智杰
// 团队：贝利珠海贼团
// 项目: 鲸鱼刷卡机
// 学校：北京理工大学珠海学院
// 邮箱：543541941@qq.com

define("TOKEN", "zhbit");
header('Content-type:text/html;charset=utf-8');
$wecharObj=new Card();
if($_GET['echostr']){
    $wecharObj->valid();
}else{
    $wecharObj->responseMsg();
}


class Card
{
    private $fromUsername;
    private $toUsername;
    private $content;  //接收的信息
    private $time; //时间戳
    private $ReContent; //回复信息
    private $state;  //状态
    private $money; //余额
    private $position; //刷卡机位置
    private $cardid;  //卡号
    private $Conn; //连接mysql


	public function valid()
    {
        $echoStr = $_GET["echostr"];

        //valid signature , option
        if($this->checkSignature()){
        	echo $echoStr;
        	exit;
        }
    }

    public function responseMsg()
    {
		//get post data, May be due to the different environments
		$postStr = $GLOBALS["HTTP_RAW_POST_DATA"];

      	//extract post data
		if (!empty($postStr)){
                /* libxml_disable_entity_loader is to prevent XML eXternal Entity Injection,
                   the best way is to check the validity of xml by yourself */
                // libxml_disable_entity_loader(true);

              	$postObj = simplexml_load_string($postStr, 'SimpleXMLElement', LIBXML_NOCDATA);
                //解析数据

                $this->fromUsername = $postObj->FromUserName;
                $this->toUsername = $postObj->ToUserName;
                $this->content = trim($postObj->Content);
                $this->time=time();

                $this->CheckUser();
                switch ($this->content) {
                    case '饭卡':
                        if($this->state==0){
                            $this->ReContent="卡号:{$this->cardid}\n状态:正常\n余额:{$this->money}";
                        }else if ($this->state==1) {
                            $this->ReContent="卡号:{$this->cardid}\n状态:冻结未被吸卡\n余额:{$this->money}";
                        }else if($this->state==11){
                            $this->ReContent="卡号:{$this->cardid}\n状态:冻结已被吸卡\n余额:{$this->money}\n吸卡地点：第{$this->position}饭堂";
                        }else{
                            $this->ReContent="查询出错";
                        }
                        break;
                    case '解冻饭卡':
                    case '饭卡解冻':
                        $this->ReContent="解冻";
                        $this->State(0);
                        break;
                    case '冻结饭卡':
                    case '饭卡冻结':
                        $this->ReContent="冻结";
                        $this->State(1);
                        break;
                }
                mysql_close($this->Conn);

                $textTpl = "<xml>
							<ToUserName><![CDATA[%s]]></ToUserName>
							<FromUserName><![CDATA[%s]]></FromUserName>
							<CreateTime>%s</CreateTime>
							<MsgType><![CDATA[%s]]></MsgType>
							<Content><![CDATA[%s]]></Content>
							<FuncFlag>0</FuncFlag>
							</xml>";             

          		$msgType = "text";
            	$resultStr = sprintf($textTpl, $this->fromUsername, $this->toUsername, $this->time, $msgType, $this->ReContent);
            	echo $resultStr;
        }else {
        	echo "连接成功！";
        	exit();
        }
    }
		
	private function checkSignature()
	{
        // you must define TOKEN by yourself
        if (!defined("TOKEN")) {
            throw new Exception('TOKEN is not defined!');
        }
        
        $signature = $_GET["signature"];
        $timestamp = $_GET["timestamp"];
        $nonce = $_GET["nonce"];
        		
		$token = TOKEN;
		$tmpArr = array($token, $timestamp, $nonce);
        // use SORT_STRING rule
		sort($tmpArr, SORT_STRING);
		$tmpStr = implode( $tmpArr );
		$tmpStr = sha1( $tmpStr );
		
		if( $tmpStr == $signature ){
			return true;
		}else{
			return false;
		}
	}

    private function getsql($dbname){
        $host ="localhost";
        $port ="3306";
        $user ="";//这里填写你用户名
        $pwd ="";//这里填写你数据库密码

        /*接着调用mysql_connect()连接服务器*/
        $this->Conn = @mysql_connect("{$host}:{$port}",$user,$pwd,true);
        if(!$this->Conn) {
                    die("Connect Server Failed: " . mysql_error($this->Conn));
                   }
        /*连接成功后立即调用mysql_select_db()选中需要连接的数据库*/
        if(!mysql_select_db($dbname,$this->Conn)) {
                    die("Select Database Failed: " . mysql_error($this->Conn));
                   }
        mysql_query("set names utf8");
    }


    //提示用户绑定
    private function note(){
        $this->ReContent = "首次查询要先绑定学号才行喔！ <a href=\"http://119.29.147.81/card/bangding.php?id={$this->fromUsername}\">→点击绑定</a>";
        $textTpl = "<xml>
                        <ToUserName><![CDATA[%s]]></ToUserName>
                        <FromUserName><![CDATA[%s]]></FromUserName>
                        <CreateTime>%s</CreateTime>
                        <MsgType><![CDATA[%s]]></MsgType>
                        <Content><![CDATA[%s]]></Content>
                        <FuncFlag>0</FuncFlag>
                        </xml>";
        $msgType = "text";
        $resultStr = sprintf ($textTpl, $this->fromUsername, $this->toUsername, $this->time, $msgType, $this->ReContent);
        echo $resultStr;
    }




    //检测用户是否绑定的函数
    private function CheckUser(){
        $this->getsql("US");
        $sql="select * from us where userid='{$this->fromUsername}'";
        $res=mysql_query($sql);
        if($row=mysql_fetch_assoc($res)){
            $this->money=$row['money'];
            $this->position=$row['cardno'];
            $this->cardid=$row['cardid'];
            $this->state=$row['state'];
        }else{
            $this->note();
            exit();
        }
    }



    //解冻和冻结饭卡
    private function State($state){
        $sql="update us set state={$state} where userid='{$this->fromUsername}'";
        if(mysql_query($sql))
        {
            $this->ReContent.="成功";
        }else{
            $this->ReContent.="失败";
        }
    }

}

?>