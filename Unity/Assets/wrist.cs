using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class wrist : MonoBehaviour {
	private Color colorStart = Color.green;
	private Color colorEnd = Color.red;
	private float duration = 1.0F;
	private Component[] renderers;


	private float twist;

	private float prediction;

	// Use this for initialization
	void Start () {
		renderers = GetComponentsInChildren(typeof(Renderer));
	}

	// Update is called once per frame
	void Update () {
//				float prediction = 60f * Input.GetAxis("K") * Time.deltaTime;

		float lerp = Mathf.PingPong(prediction, duration) / duration;

		foreach(Renderer childRenderer in renderers)
		{
			childRenderer.material.color = Color.Lerp(colorStart, colorEnd, lerp);
		}


//		float twist = -60f * Input.GetAxis("K") * Time.deltaTime * 88f;
//		float twist = -0f;
		transform.rotation = Quaternion.Euler(new Vector3(0, -90, twist));
	}

	public void TwistWrist (float amount) {
		twist = amount;
	}

	public void SetPrediction (float amount) {
		prediction = amount;
	}
}
