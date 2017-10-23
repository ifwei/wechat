<?php
// $Id$

/**
 * @file
 *集成财付通的支付服务.
 *zhupou.cn制作
 */

function uc_tenpay_show() {

	$output = "支付成功";
	return $output;
	
}

function uc_tenpay_return() {

	module_load_include('inc', 'uc_tenpay', 'classes/PayResponseHandler.class');

	/* 密钥 */
	$key = variable_get('uc_tenpay_key', '8934e7d15453e97507ef794cf7b0519d');

	/* 创建支付应答对象 */
	$resHandler = new PayResponseHandler();
	$resHandler->setKey($key);

	//判断签名
	if($resHandler->isTenpaySign()) {
		
		//交易单号
		$transaction_id = $resHandler->getParameter("transaction_id");
		
		//金额,以分为单位
		$total_fee = $resHandler->getParameter("total_fee");
		
		//支付结果
		$pay_result = $resHandler->getParameter("pay_result");
		
		//订单号
		$order_id = $resHandler->getParameter("sp_billno");
		
		if( "0" == $pay_result ) {
		
			//------------------------------
			//处理业务开始
			//------------------------------
			
			//注意交易单不要重复处理
			//注意判断返回金额
			
			//echo "success";
			//这里放入你自定义代码,比如根据不同的trade_status进行不同操作
			//log_result("verify_success"); 
			$order = uc_order_load($order_id);
			if ((uc_order_status_data($order->order_status, 'state') == 'in_checkout')  && ($total_fee >= $order->order_total * 100) ) {
				  // Empty that cart...
        //uc_cart_empty($cart_id);
				$comment = t('财付通交易ID: @transaction_id', array('@transaction_id' => $transaction_id));
				uc_payment_enter($order_id, 'tenpay', $total_fee, $order->uid, NULL, $comment);
				//uc_cart_complete_sale($order);
				uc_order_comment_save($order->order_id, 0, t('买家通过财付通即时付款@amount元.',array('@amount' => number_format($total_fee/100,2))), 'order', 'payment_received');
				//uc_order_comment_save($order->order_id, 0, t('买家通过财付通即时付款@amount元.',array('@amount' => $total_fee)));
				$output = uc_cart_complete_sale($order, variable_get('uc_new_customer_login', FALSE));
			}
			//------------------------------
			//处理业务完毕
			//------------------------------	
			
			//调用doShow, 打印meta值跟js代码,告诉财付通处理成功,并在用户浏览器显示$show页面.
			$base_path = 'http://'.$_SERVER['HTTP_HOST'].base_path();
			$show = $base_path."tenpay/show";
			$resHandler->doShow($show);
		
		} else {
			//当做不成功处理
			 $output = "<br/>" . "支付失败" . "<br/>";
		}
		
	} else {
		$output = "<br/>" . "认证签名失败" . "<br/>";
	}

	return $output;
}



